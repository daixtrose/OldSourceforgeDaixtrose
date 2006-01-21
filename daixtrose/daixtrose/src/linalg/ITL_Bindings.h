//-*-c++-*-
#ifndef DORIS_ITL_BINDINGS_H
#define DORIS_ITL_BINDINGS_H

// we use operator <= for assignment, since it makes no sense otherwise, so it
// is available for this.

#include "mtl/matrix.h"
#include "mtl/mtl.h"
#include "mtl/utils.h"

#include "linalg/Linalg.h"
#include "tiny/TinyMatAndVec.h"
#include "linalg/RowSum.h"


namespace daixt_to_mpl
{

template <typename MTL_MatrixT,
          typename DaixtMatrixT>
void
operator<=(MTL_MatrixT & mtl_matrix,
           DaixtMatrixT const & daixt_matrix)
{
  // Assumption: DaixtMatrix is Block matrix
  typedef typename DaixtMatrixT::RowStorageT RowStorageT;

  static const std::size_t block_size =
    RowStorageT::value_type::second_type::Disambiguation::Dimension;

  std::size_t nrows = block_size * Linalg::NumberOfRows(daixt_matrix);
  std::size_t ncols = block_size * Linalg::NumberOfCols(daixt_matrix);

  mtl_matrix.resize(nrows, ncols);

  for (std::size_t irow = 0; irow < daixt_matrix.nrows(); ++irow) 
    {
      RowStorageT const Row = 
        Linalg::RowExtractor<typename DaixtMatrixT::Disambiguation>
        (irow+1)(daixt_matrix);

      typedef typename RowStorageT::const_iterator const_iterator;
      
      for (const_iterator iter = Row.begin(); iter != Row.end(); ++iter)
        {
          for (std::size_t br = 0; br < block_size; ++br)
            {
              for (std::size_t bc = 0; bc < block_size; ++bc)
                {
                  mtl_matrix(irow * block_size + br,
                             (iter->first - 1) * block_size + bc)
                    =
                    (iter->second)(br+1, bc+1);
                }
            }
        }
    }
}


template <typename MTL_VectorT,
          typename BlockVectorT>
void 
operator<=(MTL_VectorT & mtl_vector,
           Linalg::Vector<BlockVectorT> const & daixt_vector)
{
  static const std::size_t block_size =
    BlockVectorT::Disambiguation::Dimension;

  const std::size_t dest_size = block_size * daixt_vector.size();

  mtl_vector.resize(dest_size);

  for (std::size_t j = 0; j < daixt_vector.size(); ++j) 
    { 
      for (std::size_t jj = 0; jj < block_size; ++jj)
        {
          mtl_vector[j * block_size + jj] = daixt_vector(j+1)(jj+1);
        }
    }
} 

} // namespace daixt_to_mpl

#endif // DORIS_ITL_BINDINGS_H
