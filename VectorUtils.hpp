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

#ifndef VectorUtils_hpp
#define VectorUtils_hpp

#include <cstdarg>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

class VectorUtils {
    
public:
    
    //sorts vector and returns vector with original position indices
    static vector<size_t> sort_indexes(const vector<double> &v);
    
    //sorts vector according to vector of indices. The vector of indices is destroyed
    template< typename order_iterator, typename value_iterator >
    static void reorder_index_destructive( order_iterator order_begin, order_iterator order_end, value_iterator v);
    
    //sorts vector according to vector of indices.
    template< typename order_iterator, typename value_iterator >
    void reorder_index( order_iterator order_begin, order_iterator order_end, value_iterator v);
    
    //filters a variable number of vectors according to their 
    
};

#endif /* VectorUtils_hpp */
