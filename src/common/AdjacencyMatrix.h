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

#if !defined(ADJACENCY_MATRIX_H)
#define ADJACENCY_MATRIX_H

#include <fstream>
#include <iostream>
#include <memory>

template<typename T>
class SquareMatrix
{
private:
  size_t size;
  std::unique_ptr<std::unique_ptr<T[]>[]> matrix;
public:
  SquareMatrix()
    : size(0)
    , matrix(nullptr)
  { }
  SquareMatrix(size_t size_, const T& t) 
    : size(size_)
    , matrix(std::make_unique< std::unique_ptr<int[]>[] >(size))
  {
    for (size_t i = 0; i < size; i++) {
      matrix[i] = std::make_unique< T[] >(size);
      for (int j = 0; j < size; j++)
        matrix[i][j] = t;
    }
  }
  ~SquareMatrix() { }
  void print(const char* filename)
  {
    std::ofstream ofs(filename);
    ofs.exceptions(std::ofstream::badbit);
    try
    {
      for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++)
          ofs << matrix[i][j] << ' ';
        ofs << '\n';
      }
    }
    catch(const std::exception& e)
    {
      std::cerr << e.what() << '\n';
    }
    ofs.close();
  }
  T& operator()(int x, int y) {
    return matrix[x][y];
  }
  template <typename U> 
  friend std::ostream& operator<<(ostream& os, const SquareMatrix<U>& m) {
    for (size_t i = 0; i < m.size-1; i++) {
      for (size_t j = 0; j < m.size; j++)
        os << m.matrix[i][j] << ' ';
      os << '\n';
    }
    for (size_t j = 0; j < m.size; j++)
      os << m.matrix[m.size-1][j] << ' ';
    return os;
  }
};


#endif // ADJACENCY_MATRIX_H
