/* mockturtle: C++ logic network library
 * Copyright (C) 2018-2019  EPFL
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*!
  \file xmg_resub.hpp
  \brief Resubstitution

  \author Heinz Riener
*/

#pragma once

#include <mockturtle/algorithms/resubstitution.hpp>
#include <mockturtle/networks/xmg.hpp>

namespace mockturtle
{

namespace detail
{

/*! \brief Ternary XOR of three truth tables */
template<typename TT>
inline TT ternary_xor( const TT& first, const TT& second, const TT& third )
{
  return kitty::ternary_operation( first, second, third, []( auto a, auto b, auto c ) { return ( ( a ^ b ) ^ c ); } );
}

template<typename TT>
inline uint64_t count_zeroes( const TT& tt )
{
  return kitty::count_ones( ~tt );
}

template<typename TT>
inline uint64_t rdb( const TT& tt )
{
  return count_zeroes( tt ) * kitty::count_ones( tt );
}

template<typename TT>
inline uint64_t db( const TT& tt, const TT& rel_tt )
{
  return kitty::count_ones( ~tt & ~rel_tt ) * kitty::count_ones( tt & rel_tt ) + kitty::count_ones( ~tt & rel_tt ) * kitty::count_ones( tt & ~rel_tt );
}

} // namespace detail

struct xmg_resub_stats
{
  /*! \brief Accumulated runtime for const-resub */
  stopwatch<>::duration time_resubC{0};

  /*! \brief Accumulated runtime for zero-resub */
  stopwatch<>::duration time_resub0{0};

  /*! \brief Accumulated runtime for one-resub */
  stopwatch<>::duration time_resub1{0};

  /*! \brief Number of accepted constant resubsitutions */
  uint32_t num_const_accepts{0};

  /*! \brief Number of accepted zero resubsitutions */
  uint32_t num_div0_accepts{0};

  /*! \brief Number of accepted one resubsitutions */
  uint32_t num_div1_accepts{0};

  uint32_t num_div1_xor3_accepts{0};
  uint32_t num_div1_xnor3_accepts{0};
  uint32_t num_div1_maj3_accepts{0};
  uint32_t num_div1_not_maj3_accepts{0};

  uint32_t num_filtered0{0};
  uint32_t num_filtered1{0};

  void report() const
  {
    fmt::print( "[i] kernel: xmg_resub_functor\n" );
    fmt::print( "[i] constant-resub {:6d}                                                           ({:>5.2f} secs)\n",
                num_const_accepts, to_seconds( time_resubC ) );
    fmt::print( "[i]        0-resub {:6d}                                                           ({:>5.2f} secs)\n",
                num_div0_accepts, to_seconds( time_resub0 ) );
    fmt::print( "[i]        1-resub {:6d} = {:6d} XOR3 + {:6d} XNOR3 + {:6d} MAJ3 + {:6d} NOT-MAJ3  ({:>5.2f} secs)\n",
                num_div1_accepts, num_div1_xor3_accepts, num_div1_xnor3_accepts, num_div1_maj3_accepts, num_div1_not_maj3_accepts,
                to_seconds( time_resub1 ) );
    fmt::print( "[i] filtering candidates: {:6d} candidates in first loop + {:6d} candidates in second loop\n", num_filtered0, num_filtered1 );
  }
}; /* xmg_resub_stats */

template<typename Ntk, typename Simulator, typename TT>
struct xmg_resub_functor
{
public:
  using node = xmg_network::node;
  using signal = xmg_network::signal;
  using stats = xmg_resub_stats;

public:
  explicit xmg_resub_functor( Ntk& ntk, Simulator const& sim, std::vector<node> const& divs, uint32_t num_divs, stats& st )
      : ntk( ntk ), sim( sim ), divs( divs ), num_divs( num_divs ), st( st )
  {
  }

  std::optional<signal> operator()( node const& root, TT care, uint32_t required, uint32_t max_inserts, uint32_t num_mffc, uint32_t& last_gain )
  {
    assert( is_const0( ~care ) );

    /* consider constants */
    auto g = call_with_stopwatch( st.time_resubC, [&]() {
      return resub_const( root, required );
    } );
    if ( g )
    {
      ++st.num_const_accepts;
      last_gain = num_mffc;
      return g; /* accepted resub */
    }

    /* consider equal nodes */
    g = call_with_stopwatch( st.time_resub0, [&]() {
      return resub_div0( root, required );
    } );
    if ( g )
    {
      ++st.num_div0_accepts;
      last_gain = num_mffc;
      return g; /* accepted resub */
    }

    if ( max_inserts == 0 || num_mffc == 1 )
      return std::nullopt;

    /* consider adding one gate */
    g = call_with_stopwatch( st.time_resub1, [&]() {
      return resub_div1( root, required );
    } );
    if ( g )
    {
      ++st.num_div1_accepts;
      last_gain = num_mffc - 1;
      return g; /* accepted resub */
    }

    return std::nullopt;
  }

  std::optional<signal> resub_const( node const& root, uint32_t required ) const
  {
    (void)required;
    auto const tt = sim.get_tt( ntk.make_signal( root ) );
    if ( tt == sim.get_tt( ntk.get_constant( false ) ) )
    {
      return sim.get_phase( root ) ? ntk.get_constant( true ) : ntk.get_constant( false );
    }
    return std::nullopt;
  }

  std::optional<signal> resub_div0( node const& root, uint32_t required ) const
  {
    (void)required;
    auto const tt = sim.get_tt( ntk.make_signal( root ) );
    for ( auto i = 0u; i < num_divs; ++i )
    {
      auto const d = divs.at( i );

      if ( tt != sim.get_tt( ntk.make_signal( d ) ) )
        continue; /* next */

      return ( sim.get_phase( d ) ^ sim.get_phase( root ) ) ? !ntk.make_signal( d ) : ntk.make_signal( d );
    }

    return std::nullopt;
  }

  struct divisor
  {
    explicit divisor( uint32_t node, int32_t entropy )
        : node( node ), entropy( entropy )
    {
    }

    uint32_t node;
    int32_t entropy;
  };

  std::optional<signal> resub_div1( node const& root, uint32_t required )
  {
    (void)required;
    auto const& tt = sim.get_tt( ntk.make_signal( root ) );
    int32_t const root_rdb = detail::rdb( tt );

    std::vector<divisor> sorted_divs;
    for ( auto it = std::begin( divs ), ie = std::begin( divs ) + num_divs; it != ie; ++it )
    {
      auto const s = ntk.make_signal( *it );
      auto const& tt_s = sim.get_tt( s );
      sorted_divs.emplace_back( *it, detail::db( tt_s, tt ) );
    }
    std::sort( std::rbegin( sorted_divs ), std::rend( sorted_divs ),
               [&]( auto const& u, auto const& v ) {
                 return u.entropy < v.entropy;
               } );

    for ( auto i = 0u; i < sorted_divs.size(); ++i )
    {
      auto const s0 = ntk.make_signal( sorted_divs.at( i ).node );
      auto const& tt0 = sim.get_tt( s0 );
      auto const a = sim.get_phase( ntk.get_node( s0 ) ) ? !s0 : s0;

      int64_t const db_s0 = sorted_divs.at( i ).entropy;
      int64_t const bound0 = root_rdb - db_s0;
      for ( auto j = i + 1; j < sorted_divs.size(); ++j )
      {
        auto const s1 = ntk.make_signal( sorted_divs.at( j ).node );
        auto const& tt1 = sim.get_tt( s1 );
        auto const b = sim.get_phase( ntk.get_node( s1 ) ) ? !s1 : s1;

        int64_t const db_s1 = sorted_divs.at( j ).entropy;
        if ( ( 2u * db_s1 ) < bound0 )
        {
          st.num_filtered0 += ( sorted_divs.size() - j - 1 ) * ( sorted_divs.size() - j );
          break;
        }

        int64_t const bound1 = bound0 - db_s1;
        for ( auto k = j + 1; k < sorted_divs.size(); ++k )
        {
          auto const s2 = ntk.make_signal( sorted_divs.at( k ).node );
          auto const& tt2 = sim.get_tt( s2 );
          auto const c = sim.get_phase( ntk.get_node( s2 ) ) ? !s2 : s2;

          int64_t const db_s2 = sorted_divs.at( k ).entropy;
          if ( db_s2 < bound1 )
          {
            st.num_filtered1 += sorted_divs.size() - k - 1;
            break;
          }

          if ( tt == detail::ternary_xor( tt0, tt1, tt2 ) )
          {
            /* XOR3 */
            ++st.num_div1_xor3_accepts;
            return sim.get_phase( root ) ? !ntk.create_xor3( a, b, c ) : ntk.create_xor3( a, b, c );
          }
          else if ( tt == detail::ternary_xor( ~tt0, tt1, tt2 ) )
          {
            /* XNOR3 */
            ++st.num_div1_xnor3_accepts;
            return sim.get_phase( root ) ? !ntk.create_xor3( !a, b, c ) : ntk.create_xor3( !a, b, c );
          }
          else if ( tt == kitty::ternary_majority( tt0, tt1, tt2 ) )
          {
            /* MAJ3 */
            ++st.num_div1_maj3_accepts;
            return sim.get_phase( root ) ? !ntk.create_maj( a, b, c ) : ntk.create_maj( a, b, c );
          }
          else if ( tt == kitty::ternary_majority( ~tt0, tt1, tt2 ) )
          {
            /* NOT-MAJ3 */
            ++st.num_div1_not_maj3_accepts;
            return sim.get_phase( root ) ? !ntk.create_maj( !a, b, c ) : ntk.create_maj( !a, b, c );
          }
        }
      }
    }
    return std::nullopt;
  }

private:
  Ntk& ntk;
  Simulator const& sim;
  std::vector<node> const& divs;
  uint32_t const num_divs;
  stats& st;
}; /* xmg_resub_functor */

template<class Ntk>
void xmg_resubstitution( Ntk& ntk, resubstitution_params const& ps = {}, resubstitution_stats* pst = nullptr )
{
  static_assert( is_network_type_v<Ntk>, "Ntk is not a network type" );
  static_assert( has_clear_values_v<Ntk>, "Ntk does not implement the clear_values method" );
  static_assert( has_fanout_size_v<Ntk>, "Ntk does not implement the fanout_size method" );
  static_assert( has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin method" );
  static_assert( has_foreach_gate_v<Ntk>, "Ntk does not implement the foreach_gate method" );
  static_assert( has_foreach_node_v<Ntk>, "Ntk does not implement the foreach_node method" );
  static_assert( has_get_constant_v<Ntk>, "Ntk does not implement the get_constant method" );
  static_assert( has_get_node_v<Ntk>, "Ntk does not implement the get_node method" );
  static_assert( has_is_complemented_v<Ntk>, "Ntk does not implement the is_complemented method" );
  static_assert( has_is_pi_v<Ntk>, "Ntk does not implement the is_pi method" );
  static_assert( has_make_signal_v<Ntk>, "Ntk does not implement the make_signal method" );
  static_assert( has_set_value_v<Ntk>, "Ntk does not implement the set_value method" );
  static_assert( has_set_visited_v<Ntk>, "Ntk does not implement the set_visited method" );
  static_assert( has_size_v<Ntk>, "Ntk does not implement the has_size method" );
  static_assert( has_substitute_node_v<Ntk>, "Ntk does not implement the has substitute_node method" );
  static_assert( has_value_v<Ntk>, "Ntk does not implement the has_value method" );
  static_assert( has_visited_v<Ntk>, "Ntk does not implement the has_visited method" );

  using resub_view_t = fanout_view<depth_view<Ntk>>;
  depth_view<Ntk> depth_view{ntk};
  resub_view_t resub_view{depth_view};

  using node_mffc_t = detail::node_mffc_inside<Ntk>;
  resubstitution_stats st;
  if ( ps.max_pis == 8 )
  {
    using truthtable_t = kitty::static_truth_table<8>;
    using truthtable_dc_t = kitty::dynamic_truth_table;
    using simulator_t = detail::simulator<resub_view_t, truthtable_t>;
    using resubstitution_functor_t = xmg_resub_functor<resub_view_t, simulator_t, truthtable_dc_t>;
    typename resubstitution_functor_t::stats resub_st;
    detail::resubstitution_impl<resub_view_t, simulator_t, resubstitution_functor_t, truthtable_dc_t, node_mffc_t> p( resub_view, ps, st, resub_st );
    p.run();
    if ( ps.verbose )
    {
      st.report();
      resub_st.report();
    }
  }
  else
  {
    using truthtable_t = kitty::dynamic_truth_table;
    using simulator_t = detail::simulator<resub_view_t, truthtable_t>;
    using resubstitution_functor_t = xmg_resub_functor<resub_view_t, simulator_t, truthtable_t>;
    typename resubstitution_functor_t::stats resub_st;
    detail::resubstitution_impl<resub_view_t, simulator_t, resubstitution_functor_t, truthtable_t, node_mffc_t> p( resub_view, ps, st, resub_st );
    p.run();
    if ( ps.verbose )
    {
      st.report();
      resub_st.report();
    }
  }

  if ( pst )
  {
    *pst = st;
  }
}

} /* namespace mockturtle */
