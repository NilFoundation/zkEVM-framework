#ifndef ZKEMV_FRAMEWORK_BIN_EMULATOR_WRAPPER_HPP_
#define ZKEMV_FRAMEWORK_BIN_EMULATOR_WRAPPER_HPP_

#ifdef __cplusplus
extern "C" {
#endif

void* make_runner();
int run(void* handler, unsigned long shardId, unsigned long blockId);

#ifdef __cplusplus
}
#endif

#endif  // ZKEMV_FRAMEWORK_BIN_EMULATOR_WRAPPER_HPP_
