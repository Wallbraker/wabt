/*
 * Copyright 2016 WebAssembly Community Group participants
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef WABT_STREAM_H_
#define WABT_STREAM_H_

#include <cassert>
#include <memory>

#include "common.h"
#include "writer.h"

namespace wabt {

/* whether to display the ASCII characters in the debug output for
 * write_memory */
enum class PrintChars {
  No = 0,
  Yes = 1,
};

class Stream {
 public:
  Stream(Writer* writer, Stream* log_stream = nullptr);

  static Stream* Stdout();
  static Stream* Stderr();

  // Helper functions for writing to a Stream. the |desc| parameter is
  // optional, and will be appended to the log stream if |log_stream| is
  // non-null.
  void WriteDataAt(size_t offset,
                   const void* src,
                   size_t size,
                   PrintChars print_chars = PrintChars::No,
                   const char* desc = nullptr);
  void WriteData(const void* src, size_t size, const char* desc = nullptr);
  void MoveData(size_t dst_offset, size_t src_offset, size_t size);

  void WABT_PRINTF_FORMAT(2, 3) Writef(const char* format, ...);

  // Specified as uint32_t instead of uint8_t so we can check if the value
  // given is in range before wrapping.
  void WriteU8(uint32_t value, const char* desc = nullptr) {
    assert(value <= UINT8_MAX);
    Write(static_cast<uint8_t>(value), desc);
  }
  void WriteU32(uint32_t value, const char* desc = nullptr) {
    Write(value, desc);
  }
  void WriteU64(uint64_t value, const char* desc = nullptr) {
    Write(value, desc);
  }
  void WriteChar(char c) { WriteU8(c); }

  // Dump memory as text, similar to the xxd format.
  void WriteMemoryDump(const void* start,
                       size_t size,
                       size_t offset,
                       PrintChars print_chars = PrintChars::No,
                       const char* prefix = nullptr,
                       const char* desc = nullptr);

  void WriteOutputBufferMemoryDump(const OutputBuffer& buf);

  // Convenience functions for writing enums.
  template <typename T>
  void WriteU8Enum(T value, const char* desc = nullptr) {
    WriteU8(static_cast<uint32_t>(value), desc);
  }

 private:
  template <typename T>
  void Write(const T& data, const char* desc = nullptr) {
    WriteData(&data, sizeof(data), desc);
    offset_ += sizeof(data);
  }

  Writer* writer_;  // Not owned.
  size_t offset_;
  Result result_;
  // Not owned. If non-null, log all writes to this stream.
  Stream* log_stream_;
};

}  // namespace wabt

#endif /* WABT_STREAM_H_ */
