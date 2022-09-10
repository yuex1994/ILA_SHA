/// \file the ila model of SHA top module
///  Yue Xing (yuex@princeton.edu)
///

#include "sha_ila.h"

SHA::SHA()
    : // construct the model
      model("SHA"),
      // I/O interface: this is where the commands come from.
      cmd    (model.NewBvInput("cmd"    , 2 )), 
      cmdaddr(model.NewBvInput("cmdaddr", 16)),
      cmddata(model.NewBvInput("cmddata", 8 )),
      // internal arch state.
      dataout(model.NewBvState("dataout", 8)),
      state(model.NewBvState("sha_state", 3)),
      rdaddr(model.NewBvState("sha_rdaddr", 16)),
      wraddr(model.NewBvState("sha_wraddr", 16)),
      oplen(model.NewBvState("sha_len", 16)),
      // the memory: shared state
      xram   (model.NewMemState("XRAM"      , 16, 8)),
      // The SHA function :
      sha_f(FuncRef("sha_f",
                     SortRef::BV(160),  
                     SortRef::BV(512))){

  // SHA fetch function -- what corresponds to instructions
  model.SetFetch(Concat(cmd, Concat(cmdaddr, cmddata)));
  // Valid instruction: cmd == 1 or cmd == 2
  model.SetValid((cmd == 1) | (cmd == 2));

  // some shortcuts
  auto is_status_idle = state == SHA_STATE_IDLE;

  // add instructions

  { // WRITE_RD_ADDR
    auto instr = model.NewInstr("WRITE_RD_ADDR");

    instr.SetDecode(
      (cmd == CMD_WRITE) & (cmdaddr >= SHA_RD_ADDR) & (cmdaddr < SHA_RD_ADDR + 2));

    instr.SetUpdate(rdaddr,
      Ite(is_status_idle, // Check if it is idle
          Concat(         // if idle, update one slice of the register at a time
            Ite(cmdaddr == SHA_RD_ADDR + 1, cmddata, rdaddr(15,8) ), // the upper 8-bits
            Ite(cmdaddr == SHA_RD_ADDR    , cmddata, rdaddr( 7,0) )  // the lower 8-bits
          ),
          rdaddr        // if not idle, no change
        )); // update a slice of the register. Slice selected by the cmd address

  }
  { // WRITE_WR_ADDR
    auto instr = model.NewInstr("WRITE_WR_ADDR");

    instr.SetDecode(
      (cmd == CMD_WRITE) & (cmdaddr >= SHA_WR_ADDR) & (cmdaddr < SHA_WR_ADDR + 2));

    instr.SetUpdate(wraddr,
      Ite(is_status_idle, // Check if it is idle
          Concat(         // if idle, update one slice of the register at a time
            Ite(cmdaddr == SHA_WR_ADDR + 1, cmddata, wraddr(15,8) ), // the upper 8-bits
            Ite(cmdaddr == SHA_WR_ADDR    , cmddata, wraddr( 7,0) )  // the lower 8-bits
          ),
          wraddr        // if not idle, no change
        )); // update a slice of the register. Slice selected by the cmd address
  }
  { // WRITE_len
    auto instr = model.NewInstr("WRITE_LEN");

    instr.SetDecode(
      (cmd == CMD_WRITE) & (cmdaddr >= SHA_LEN) & (cmdaddr < SHA_LEN + 2));

    instr.SetUpdate(oplen,
      Ite(is_status_idle, // Check if it is idle
          Concat(         // if idle, update one slice of the register at a time
            Ite(cmdaddr == SHA_LEN + 1, cmddata, oplen(15,8) ), // the upper 8-bits
            Ite(cmdaddr == SHA_LEN    , cmddata, oplen( 7,0) )  // the lower 8-bits
          ),
          oplen        // if not idle, no change
        )); // update a slice of the register. Slice selected by the cmd address

  }


  { // READ_RD_ADDR
    auto instr = model.NewInstr("READ_RD_ADDR");

    instr.SetDecode(
      (cmd == CMD_READ) & (cmdaddr >= SHA_RD_ADDR) & (cmdaddr < SHA_RD_ADDR + 2));
    instr.SetUpdate(dataout, slice_read(rdaddr, cmdaddr, SHA_RD_ADDR, 2, 8));
  }

  { // READ_WR_ADDR
    auto instr = model.NewInstr("READ_WR_ADDR");

    instr.SetDecode(
      (cmd == CMD_READ) & (cmdaddr >= SHA_WR_ADDR) & (cmdaddr < SHA_WR_ADDR + 2));

    instr.SetUpdate(dataout, slice_read(wraddr, cmdaddr, SHA_WR_ADDR, 2, 8));
  }

  { // READ_len
    auto instr = model.NewInstr("READ_LEN");

    instr.SetDecode(
      (cmd == CMD_READ) & (cmdaddr >= SHA_LEN) & (cmdaddr < SHA_LEN + 2));

    instr.SetUpdate(dataout, slice_read(oplen, cmdaddr, SHA_LEN, 2, 8));
  }

 
  { // START_SHA
    auto instr = model.NewInstr("START_SHA");

    instr.SetDecode((cmd == CMD_WRITE) & (cmdaddr == SHA_START) &
                    (cmddata == 1));
    // if it is idle, we will start, if it is not idle, there is no guarantee
    // what it may become
    instr.SetUpdate(state, Ite(is_status_idle, BvConst(1, 2), unknown(2)()));

    // AddChild(instr);
  }

    /// add child
}
