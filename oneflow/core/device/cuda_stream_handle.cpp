#include "oneflow/core/device/cuda_stream_handle.h"
#include "oneflow/core/device/cuda_util.h"

namespace oneflow {

#ifdef WITH_CUDA

const cudaStream_t* CudaStreamHandle::cuda_stream() {
  if (!cuda_stream_) {
    cuda_stream_.reset(new cudaStream_t);
    CudaCheck(cudaStreamCreate(cuda_stream_.get()));
  }
  return cuda_stream_.get();
}

const cublasHandle_t* CudaStreamHandle::cublas_handle() {
  if (!cublas_handle_) {
    cublas_handle_.reset(new cublasHandle_t);
    CudaCheck(cublasCreate(cublas_handle_.get()));
    CudaCheck(cublasSetStream(*cublas_handle_, *cuda_stream()));
  }
  return cublas_handle_.get();
}

#ifdef WITH_CUDNN
const cudnnHandle_t* CudaStreamHandle::cudnn_handle() {
  if (!cudnn_handle_) {
    cudnn_handle_.reset(new cudnnHandle_t);
    CudaCheck(cudnnCreate(cudnn_handle_.get()));
    CudaCheck(cudnnSetStream(*cudnn_handle_, *cuda_stream()));
  }
  return cudnn_handle_.get();
}
#endif  // WITH_CUDNN

CudaStreamHandle::~CudaStreamHandle() {
#ifdef WITH_CUDNN
  if (cudnn_handle_) { CudaCheck(cudnnDestroy(*cudnn_handle_)); }
#endif  // WITH_CUDNN
  if (cublas_handle_) { CudaCheck(cublasDestroy(*cublas_handle_)); }
  if (cuda_stream_) { CudaCheck(cudaStreamDestroy(*cuda_stream_)); }
}

#endif  // WITH_CUDA

}  // namespace oneflow
