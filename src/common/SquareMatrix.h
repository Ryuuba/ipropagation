//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//


// This class template models a generic square matrix

#if !defined(SQUARE_MATRIX_H)
#define SQUARE_MATRIX_H

#include <fstream>
#include <iostream>
#include <memory>

template<typename T>
class SquareMatrix
{
private:
  size_t size_;
  std::shared_ptr<std::shared_ptr<T[]>[]> matrix;
public:
  SquareMatrix()
    : size_(0)
    , matrix(nullptr)
  { }
  
  SquareMatrix(size_t size_, const T& t = T()) 
    : size_(size_)
    , matrix(std::make_unique< std::shared_ptr<T[]>[] >(size_))
  {
    for (size_t i = 0; i < size_; i++) {
      matrix[i] = std::make_unique< T[] >(size_);
      for (size_t j = 0; j < size_; j++)
        matrix[i][j] = t;
    }
  }

  ~SquareMatrix() { }

  T& operator()(size_t x, size_t y) {
    return matrix[x][y];
  }

  const T& operator()(size_t x, size_t y) const {
    return matrix[x][y];
  }

  size_t size() { return size_; }

  void print(const char* filename)
  {
    std::ofstream ofs(filename);
    ofs.exceptions(std::ofstream::badbit);
    try
    {
      for (size_t i = 0; i < size_; i++) {
        for (size_t j = 0; j < size_; j++)
          ofs << matrix(i, j) << ' ';
        ofs << '\n';
      }
    }
    catch(const std::exception& e)
    {
      std::cerr << e.what() << '\n';
    }
    ofs.close();
  }

  template <typename U> 
  friend std::ostream& operator<<(std::ostream& os, const SquareMatrix<U>& m) {
    for (size_t i = 0; i < m.size_-1; i++) {
      for (size_t j = 0; j < m.size_; j++)
        os << m(i, j) << ' ';
      os << '\n';
    }
    for (size_t j = 0; j < m.size_; j++)
      os << m(m.size_-1, j) << ' ';
    return os;
  }
};


#endif // SQUARE_MATRIX_H
