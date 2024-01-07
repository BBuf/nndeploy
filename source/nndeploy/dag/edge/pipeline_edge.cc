#include "nndeploy/dag/edge/pipeline_edge.h"

namespace nndeploy {
namespace dag {

TypeEdgeRegister<TypeEdgeCreator<PipelineEdge>> g_pipeline_edge_register(
    kEdgeTypePipeline);

PipelineEdge::PipelineEdge(ParallelType paralle_type,
                           std::initializer_list<Node *> producers,
                           std::initializer_list<Node *> consumers)
    : AbstractEdge(paralle_type, producers, consumers) {
  consumers_count_ = consumers.size();
  for (auto iter : consumers) {
    consumed_.insert({iter, 0});
  }
}

PipelineEdge::~PipelineEdge() {
  consumers_count_ = -1;

  for (auto iter : data_packets_) {
    delete iter;
  }
  data_packets_.clear();

  consumed_.clear();
}

base::Status PipelineEdge::set(device::Buffer *buffer, int index,
                               bool is_external) {
  DataPacket *dp = new DataPacket();
  dp->set(buffer, index, is_external);

  // 上锁
  std::lock_guard<std::mutex> lock(lock_);
  data_packets_.push_back({dp, 0});
  // 通知所有等待的线程
  cv_.notify_all();
}
base::Status PipelineEdge::set(device::Buffer &buffer, int index,
                               bool is_external);
base::Status PipelineEdge::create(device::Device *device,
                                  const device::BufferDesc &desc, int index);
virtual device::Buffer *getBuffer(const Node *comsumer) {
  std::lock_guard<std::mutex> lock(lock_);  // 锁上互斥锁
  // 等待通知，释放锁，被唤醒后重新上锁
  cv_.wait(lock, [this] { return !data_packets_.empty(); });

  DataPacket *tmp = nullptr;

  // 该edge没有消费者，即为输出edge
  if (comsumer == nullptr) {
    if (consumed_.empty()) {
      for (auto iter = data_packets_.begin(); iter != data_packets_.end();
           ++iter) {
        iter.second++;
        if (iter.second == 1) {
          tmp = iter.first;
          break;
        }
      }
    } else {
      NNDEPLOY_LOGE("This edge has consumer.\n");
      return nullptr;
    }
  } else {  // 该edge有消费者，即不为输出edge
    /**
     * @brief
     * #. 检测node是否合理
     * ## 不合理，报错
     * ## 合理
     * ### 检测是否有数据包
     * #### 没有数据包，报错
     * #### 有数据包，拿到数据
     */
    // check
    auto iter = consumed_.find(comsumer);
    if (iter == consumed_.end()) {
      NNDEPLOY_LOGE("This comsumer[%s] is error.\n",
                    comsumer->getName().c_str());
      return nullptr;
    } else {
      int index = iter->second;
      if (index >= data_packets_.size()) {
        NNDEPLOY_LOGE("This comsumer[%s]'s index[%d] is error.\n",
                      comsumer->getName().c_str(), index);
        return nullptr;
      }
      auto iter = data_packets_.begin();
      for (int i = 0; i <= index; i++) {
        if (iter.second == consumers_count_) {
          iter.second++;
        }
        iter++;
      }
      iter.second++;
      tmp = iter.first;
    }
  }

  // 移除数据包
  for (auto iter = data_packets_.begin(); iter != data_packets_.end(); ++iter) {
    if (iter.second > consumers_count_) {
      delete iter.first;
      data_packets_.erase(iter);
    }
  }

  return tmp->getBuffer();
}

base::Status PipelineEdge::set(device::Mat *mat, int index, bool is_external);
base::Status PipelineEdge::set(device::Mat &mat, int index, bool is_external);
base::Status PipelineEdge::create(device::Device *device,
                                  const device::MatDesc &desc, int index);
virtual device::Mat *getMat(const Node *comsumer);

#ifdef ENABLE_NNDEPLOY_OPENCV
base::Status PipelineEdge::set(cv::Mat *cv_mat, int index, bool is_external);
base::Status PipelineEdge::set(cv::Mat &cv_mat, int index, bool is_external);
virtual cv::Mat *getCvMat(const Node *comsumer);
#endif

base::Status PipelineEdge::set(device::Tensor *tensor, int index,
                               bool is_external);
base::Status PipelineEdge::set(device::Tensor &tensor, int index,
                               bool is_external);
base::Status PipelineEdge::create(device::Device *device,
                                  const device::TensorDesc &desc, int index);
virtual device::Tensor *getTensor(const Node *comsumer);

base::Status PipelineEdge::set(base::Param *param, int index, bool is_external);
base::Status PipelineEdge::set(base::Param &param, int index, bool is_external);
virtual base::Param *getParam(const Node *comsumer);

base::Status PipelineEdge::set(void *anything, int index, bool is_external);
virtual void *getAnything(const Node *comsumer);

virtual int getIndex(const Node *comsumer);

DataPacket *PipelineEdge::getDataPacket(const Node *comsumer) {
  // 锁上互斥锁
  std::lock_guard<std::mutex> lock(lock_);
  // 等待通知，释放锁，被唤醒后重新上锁
  cv_.wait(lock);
  // 返回值
  DataPacket *tmp = nullptr;
  // 该edge没有消费者，即为输出edge
  if (comsumer == nullptr) {
    if (consumed_.empty()) {
      for (auto iter = data_packets_.begin(); iter != data_packets_.end();
           ++iter) {
        iter.second++;
        if (iter.second == 2) {
          delete iter.first;
          data_packets_.erase(iter);
        } else if (iter.second == 1) {
          tmp = iter.first;
          break;
        } else {
          continue;
        }
      }
    } else {
      NNDEPLOY_LOGE("This edge has consumer.\n");
      return nullptr;
    }
  } else {  // 该edge有消费者，即不为输出edge
    /**
     * @brief
     * #. 检测node是否合理
     * ## 不合理，报错
     * ## 合理
     * ### 检测是否有数据包
     * #### 没有数据包，报错
     * #### 有数据包，拿到数据
     */
    // check
    auto iter = consumed_.find(comsumer);
    if (iter == consumed_.end()) {
      NNDEPLOY_LOGE("This comsumer[%s] is error.\n",
                    comsumer->getName().c_str());
      return nullptr;
    } else {
      int index = iter->second;
      if (index >= data_packets_.size()) {
        NNDEPLOY_LOGE("This comsumer[%s]'s index[%d] is error.\n",
                      comsumer->getName().c_str(), index);
        return nullptr;
      }
      auto iter = data_packets_.begin();
      for (int i = 0; i <= index; i++) {
        if (iter.second == consumers_count_) {
          iter.second++;
        }
        iter++;
      }
      iter.second++;
      tmp = iter.first;
    }
  }

  // 移除数据包
  for (auto iter = data_packets_.begin(); iter != data_packets_.end(); ++iter) {
    if (iter.second > consumers_count_) {
      delete iter.first;
      data_packets_.erase(iter);
    }
  }

  return tmp;
}

}  // namespace dag
}  // namespace nndeploy