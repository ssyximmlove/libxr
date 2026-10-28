#include "linux_can_error.hpp"

#include <cstdio>

#include "test_assert.hpp"

int main()
{
  using ErrorID = LibXR::CAN::ErrorID;
  struct ProtocolCase
  {
    uint8_t bits;
    ErrorID expected;
  };
  const ProtocolCase cases[] = {
      {CAN_ERR_PROT_STUFF, ErrorID::CAN_ERROR_ID_STUFF},
      {CAN_ERR_PROT_FORM, ErrorID::CAN_ERROR_ID_FORM},
      {CAN_ERR_PROT_BIT0, ErrorID::CAN_ERROR_ID_BIT0},
      {CAN_ERR_PROT_BIT1, ErrorID::CAN_ERROR_ID_BIT1},
      {CAN_ERR_PROT_BIT, ErrorID::CAN_ERROR_ID_OTHER},
      {CAN_ERR_PROT_OVERLOAD, ErrorID::CAN_ERROR_ID_OTHER},
      {0, ErrorID::CAN_ERROR_ID_PROTOCOL},
      {CAN_ERR_PROT_STUFF | CAN_ERR_PROT_FORM, ErrorID::CAN_ERROR_ID_STUFF},
  };
  unsigned checks = 0;
  for (const auto& test : cases)
  {
    canfd_frame frame{};
    // 控制器状态与协议字段故意冲突，防止回归到读取 data[1]。
    // Conflicting controller/protocol fields catch accidental reads from data[1].
    frame.data[1] =
        test.bits == CAN_ERR_PROT_STUFF ? CAN_ERR_PROT_FORM : CAN_ERR_PROT_STUFF;
    frame.data[2] = test.bits;
    TEST_ASSERT(LibXR::LinuxCANDetail::ProtocolErrorID(frame, 3) == test.expected);
    ++checks;
  }
  for (uint8_t length = 0; length <= 3; ++length)
  {
    canfd_frame frame{};
    frame.data[1] = CAN_ERR_PROT_FORM;
    frame.data[2] = CAN_ERR_PROT_STUFF;
    const auto expected =
        length < 3 ? ErrorID::CAN_ERROR_ID_PROTOCOL : ErrorID::CAN_ERROR_ID_STUFF;
    TEST_ASSERT(LibXR::LinuxCANDetail::ProtocolErrorID(frame, length) == expected);
    ++checks;
  }
  std::printf("linux_can_error: %u checks passed\n", checks);
  return 0;
}
