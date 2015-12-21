//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2015  by the igatools authors (see authors.txt).
//
// This file is part of the igatools library.
//
// The igatools library is free software: you can use it, redistribute
// it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//-+--------------------------------------------------------------------

#include <igatools/utils/safe_stl_vector.h>
#include <igatools/base/exceptions.h>

IGA_NAMESPACE_OPEN


template<class T>
T &
SafeSTLVector<T>::
operator[](Size n)
{
  Assert(n < this->size(), ExcIndexRange(n, 0, this->size()));
  return std::vector<T>::operator[](n);
}

template<class T>
const T &
SafeSTLVector<T>::
operator[](Size n) const
{
  Assert(n < this->size(), ExcIndexRange(n, 0, this->size()));
  return std::vector<T>::operator[](n);
}


IGA_NAMESPACE_CLOSE

#include <igatools/utils/safe_stl_vector.inst>
