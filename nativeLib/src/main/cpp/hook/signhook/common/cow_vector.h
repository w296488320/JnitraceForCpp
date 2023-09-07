//
// Created by SwiftGan on 2020/9/14.
//

#pragma once

#include "platform/memory.h"

namespace SandHook {

    template<typename T, bool executable = false>
    class CowVector {
    public:
        constexpr CowVector() = default;

        explicit CowVector(std::size_t count) : size_{count * sizeof(T)} {
            if (count > 0) {
                data_rx_ = reinterpret_cast<T *>(executable ? Platform::MapExecutableMemory(size_)
                                                            : Platform::MapCowMemory(size_));
                data_rw_ = executable ? reinterpret_cast<T *>(Platform::MirrorRWMemory(data_rx_, size_))
                                      : data_rx_;
            }
        }

        ~CowVector() {
            Platform::UnMapMemory(reinterpret_cast<VAddr>(data_rx_), size_);
            if (data_rw_ != data_rx_) {
                Platform::UnMapMemory(reinterpret_cast<VAddr>(data_rw_), size_);
            }
        }

        void Resize(std::size_t count) {
            if (size_) {
                Platform::UnMapMemory(reinterpret_cast<VAddr>(data_rx_), size_);
            }
            size_ = count * sizeof(T);
            data_rx_ = reinterpret_cast<T *>(executable ? Platform::MapExecutableMemory(size_)
                                                        : Platform::MapCowMemory(size_));
            data_rw_ = executable ? reinterpret_cast<T *>(Platform::MirrorRWMemory(data_rx_, size_))
                                  : data_rx_;
        }

        constexpr const T &operator[](std::size_t index) const {
            return data_rw_[index];
        }

        constexpr T &operator[](std::size_t index) {
            return data_rw_[index];
        }

        constexpr T *DataRW() {
            return data_rw_;
        }

        constexpr const T *DataRW() const {
            return data_rw_;
        }

        constexpr T *DataRX() {
            return data_rx_;
        }

        constexpr const T *DataRX() const {
            return data_rx_;
        }

        constexpr std::size_t Size() const {
            return size_ / sizeof(T);
        }

    private:
        size_t size_{};
        T *data_rx_{};
        T *data_rw_{};
    };

}
