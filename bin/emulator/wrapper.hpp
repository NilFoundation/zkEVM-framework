#ifndef ZKEMV_FRAMEWORK_BIN_EMULATOR_WRAPPER_HPP_
#define ZKEMV_FRAMEWORK_BIN_EMULATOR_WRAPPER_HPP_

#ifdef __cplusplus
extern "C" {
#endif

void* make_runner();
int run(void* handler, unsigned long shardId, const char* blockHash, const char* block_file_name);

#ifdef __cplusplus
}
#endif

#endif  // ZKEMV_FRAMEWORK_BIN_EMULATOR_WRAPPER_HPP_
