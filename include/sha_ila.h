/// \file the ila example of SHA
///  Yue Xing (yuex@princeton.edu)
///

#ifndef SHA_ILA_H__
#define SHA_ILA_H__

#include <ilang/ilang++.h>

using namespace ilang;

#define SHA_START 0xfe00
#define SHA_STATE 0xfe01
#define SHA_RD_ADDR 0xfe02
#define SHA_WR_ADDR 0xfe04
#define SHA_LEN 0xfe06
#define SHA_ADDR_END 0xfe10

#define CMD_NOP 0
#define CMD_READ 1
#define CMD_WRITE 2

#define SHA_STATE_IDLE 0
#define SHA_STATE_READ_DATA 1
#define SHA_STATE_OP1 2
#define SHA_STATE_OP2 3
#define SHA_STATE_WRITE_DATA 4

/// \brief the class of SHA ila
class SHA {

public:
  // --------------- CONSTRUCTOR ------ //
  /// add state, add instructions, add child
  SHA();
  // --------------- MEMBERS ----------- //
  /// the ila mode
  Ila model;

private:
  /// Called by the constructor to create the child-ILA
  /// for block encryption
  void AddChild(InstrRef& inst);

protected:
  // --------------- HELPERS -------- //
  /// To get a slide from the expression: reg
  /// \param[in] reg: the register to slice
  /// \param[in] idx: the address/index to select the slice
  /// \param[in] base_addr: the address to be substracted from address
  /// \param[in] no_slice: the number of slices
  /// \param[in] slice_width: the width of slice
  /// \return  the resulted read expression
  static ExprRef slice_read(const ExprRef& reg, const ExprRef& idx,
                            unsigned long base_addr, unsigned no_slice,
                            unsigned slice_width);

  /// update only part (slices) of a register
  /// \param[in] reg: the register to slice
  /// \param[in] idx: the address/index to select the slice
  /// \param[in] input_slice: the slice used to update
  /// \param[in] base_addr: the address to be substracted from address
  /// \param[in] no_slice: the number of slices
  /// \param[in] slice_width: the width of slice
  /// \return  the resulted read expression
  /// it assumes:
  /// input_slice.width == slice_width
  /// no_slice * slice_width == reg.width
  static ExprRef slice_update(const ExprRef& reg, const ExprRef& idx,
                              const ExprRef& input_slice,
                              unsigned long base_addr, unsigned no_slice,
                              unsigned slice_width);

  /// specify a nondeterministic value within range [low,high]
  ExprRef unknown_range(unsigned low, unsigned high);
  /// a nondeterministic choice of a or b
  static ExprRef unknown_choice(const ExprRef& a, const ExprRef& b);
  /// a nondeterminstic bitvector const of width
  static FuncRef unknown(unsigned width);

protected:
  // ------------ STATE ------------ //
  // I/O interface: this is where the commands come from.
  ExprRef cmd;
  ExprRef cmdaddr;
  ExprRef cmddata;
  // internal arch state.
  ExprRef state;
  ExprRef rdaddr;
  ExprRef wraddr;
  ExprRef oplen;
  ExprRef dataout;
 
  // the memory
  ExprRef xram;
  // The uninterpreted function :
  FuncRef sha_f;
  // the output

}; // class SHA

#endif // SHA_ILA_H__
