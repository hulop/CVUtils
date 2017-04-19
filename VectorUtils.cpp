/*******************************************************************************
 * Copyright (c) 2017  IBM Corporation, Carnegie Mellon University and others
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *******************************************************************************/

#include "VectorUtils.hpp"

vector<size_t> VectorUtils::sort_indexes(const vector<double> &v) {
    
    // initialize original index locations
    vector<size_t> idx(v.size());
    iota(idx.begin(), idx.end(), 0);
    
    // sort indexes based on comparing values in v
    sort(idx.begin(), idx.end(),
         [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});
    
    return idx;
}

template< typename order_iterator, typename value_iterator >
void VectorUtils::reorder_index_destructive( order_iterator order_begin, order_iterator order_end, value_iterator v )  {
    typedef typename iterator_traits< value_iterator >::value_type value_t;
    typedef typename iterator_traits< order_iterator >::value_type index_t;
    typedef typename iterator_traits< order_iterator >::difference_type diff_t;
    
    diff_t remaining = order_end - 1 - order_begin;
    for ( index_t s = index_t(); remaining > 0; ++ s ) {
        index_t d = order_begin[s];
        if ( d == (diff_t) -1 ) continue;
        -- remaining;
        value_t temp = v[s];
        for ( index_t d2; d != s; d = d2 ) {
            swap( temp, v[d] );
            swap( order_begin[d], d2 = (diff_t) -1 );
            -- remaining;
        }
        v[s] = temp;
    }
}

template< typename order_iterator, typename value_iterator >
void VectorUtils::reorder_index( order_iterator order_begin, order_iterator order_end, value_iterator v )  {
    typedef typename iterator_traits< value_iterator >::value_type value_t;
    typedef typename iterator_traits< order_iterator >::value_type index_t;
    typedef typename iterator_traits< order_iterator >::difference_type diff_t;
    
    diff_t remaining = order_end - 1 - order_begin;
    for ( index_t s = index_t(), d; remaining > 0; ++ s ) {
        for ( d = order_begin[s]; d > s; d = order_begin[d] ) ;
        if ( d == s ) {
            -- remaining;
            value_t temp = v[s];
            while ( d = order_begin[d], d != s ) {
                swap( temp, v[d] );
                -- remaining;
            }
            v[s] = temp;
        }
    }
}
