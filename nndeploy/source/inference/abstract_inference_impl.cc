
#include "nndeploy/source/inference/abstract_inference_impl.h"

namespace nndeploy {
namespace inference {

AbstractInferenceImpl::~AbstractInferenceImpl() {}

Config AbstractInferenceImpl::getConfig() { return config_; }

base::Status AbstractInferenceImpl::getMinShape(base::ShapeMap &shape_map) {
  shape_map = min_shape_;
  return base::kStatusCodeOk;
}
base::Status AbstractInferenceImpl::getOptShape(base::ShapeMap &shape_map) {
  shape_map = opt_shape_;
  return base::kStatusCodeOk;
}
base::Status AbstractInferenceImpl::getCurentShape(base::ShapeMap &shape_map) {
  shape_map = current_shape_;
  return base::kStatusCodeOk;
}
base::Status AbstractInferenceImpl::getMaxShape(base::ShapeMap &shape_map) {
  shape_map = max_shape_;
  return base::kStatusCodeOk;
}

base::Status AbstractInferenceImpl::setDevice(device::Device *device) {
  device_.push_back(device);
  return base::kStatusCodeOk;
}
device::Device *AbstractInferenceImpl::getDevice() {
  if (device_.empty()) {
    return nullptr;
  } else {
    return device_[0];
  }
}
device::Device *AbstractInferenceImpl::getDevice(int index) {
  if (index < 0 || index >= device_.size()) {
    return nullptr;
  } else {
    return device_[index];
  }
}

base::Status AbstractInferenceImpl::setBufferPool(
    device::BufferPool *buffer_pool) {
  buffer_pool_.push_back(buffer_pool);
  return base::kStatusCodeOk;
}
device::BufferPool *AbstractInferenceImpl::getBufferPool() {
  if (buffer_pool_.empty()) {
    return nullptr;
  } else {
    return buffer_pool_[0];
  }
}
device::BufferPool *AbstractInferenceImpl::getBufferPool(int index) {
  if (index < 0 || index >= buffer_pool_.size()) {
    return nullptr;
  } else {
    return buffer_pool_[index];
  }
}

int64_t AbstractInferenceImpl::getWorkspaceSize() {
  NNDEPLOY_LOGI("this api is not implemented");
  return -1;
}
int64_t AbstractInferenceImpl::getWorkspaceSize(int index) {
  NNDEPLOY_LOGI("this api is not implemented");
  return -1;
}
base::Status AbstractInferenceImpl::setWorkspace(device::Buffer *buffer) {
  NNDEPLOY_LOGI("this api is not implemented");
  return base::kStatusCodeOk;
}

int64_t AbstractInferenceImpl::getMemorySize() {
  NNDEPLOY_LOGI("this api is not implemented");
  return -1;
}
int64_t AbstractInferenceImpl::getMemorySize(int index) {
  NNDEPLOY_LOGI("this api is not implemented");
  return -1;
}
base::Status AbstractInferenceImpl::setMemory(device::Buffer *buffer) {
  NNDEPLOY_LOGI("this api is not implemented");
  return base::kStatusCodeOk;
}

device::TensorMap AbstractInferenceImpl::getAllInputTensor() {
  return current_input_tensors_;
}
device::TensorMap AbstractInferenceImpl::getAllOutputTensor() {
  return current_output_tensors_;
}

int AbstractInferenceImpl::getNumOfInputTensor() {
  return current_input_tensors_.size();
}
int AbstractInferenceImpl::getNumOfOutputTensor() {
  return current_output_tensors_.size();
}

std::vector<std::string> AbstractInferenceImpl::getInputTensorNames() {
  std::vector<std::string> input_tensor_names;
  for (auto &tensor : current_input_tensors_) {
    input_tensor_names.push_back(tensor.first);
  }
  return input_tensor_names;
}
std::vector<std::string> AbstractInferenceImpl::getOutputTensorNames() {
  std::vector<std::string> output_tensor_names;
  for (auto &tensor : current_output_tensors_) {
    output_tensor_names.push_back(tensor.first);
  }
  return output_tensor_names;
}

std::shared_ptr<device::Tensor> AbstractInferenceImpl::getInputTensor(
    const std::string &name) {
  if (current_input_tensors_.count(name) > 0) {
    return current_input_tensors_[name];
  } else {
    return nullptr;
  }
}
std::shared_ptr<device::Tensor> AbstractInferenceImpl::getOutputTensor(
    const std::string &name) {
  if (current_output_tensors_.count(name) > 0) {
    return current_output_tensors_[name];
  } else {
    return nullptr;
  }
}

std::map<base::InferenceType, std::shared_ptr<InferenceCreator>> &
getGlobalInferenceCreatorMap() {
  static std::once_flag once;
  static std::shared_ptr<
      std::map<base::InferenceType, std::shared_ptr<InferenceCreator>>>
      creators;
  std::call_once(once, []() {
    creators.reset(
        new std::map<base::InferenceType, std::shared_ptr<InferenceCreator>>);
  });
  return *creators;
}

AbstractInferenceImpl *createInference(base::InferenceType type) {
  AbstractInferenceImpl *temp = nullptr;
  auto &creater_map = getGlobalInferenceCreatorMap();
  if (creater_map.count(type) > 0) {
    temp = creater_map[type]->createInference();
  }
  return temp;
}

}  // namespace inference
}  // namespace nndeploy
