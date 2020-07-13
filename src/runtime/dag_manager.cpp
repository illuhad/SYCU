/*
 * This file is part of hipSYCL, a SYCL implementation based on CUDA/HIP
 *
 * Copyright (c) 2019 Aksel Alpay
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <memory>

#include "hipSYCL/runtime/dag_manager.hpp"
#include "hipSYCL/runtime/hints.hpp"
#include "hipSYCL/runtime/operations.hpp"
#include "hipSYCL/runtime/util.hpp"
#include "hipSYCL/runtime/dag_interpreter.hpp"

namespace hipsycl {
namespace rt {

namespace {
constexpr int max_cached_dag_nodes = 100;
}


dag_build_guard::~dag_build_guard()
{
  _mgr->trigger_flush_opportunity();
}

dag_manager::dag_manager()
: _builder{std::make_unique<dag_builder>(execution_hints{})}
{}

dag_manager::~dag_manager()
{
  wait();
}

dag_builder* 
dag_manager::builder() const
{
  return _builder.get();
}

void dag_manager::flush_async()
{
  _worker([this](){
    if(_builder->get_current_dag_size() > 0){
      // Construct new DAG
      dag new_dag = _builder->finish_and_reset();
      
      // Release any old users of memory buffers used in this dag
      for(dag_node_ptr req : new_dag.get_memory_requirements()){
        assert_is<memory_requirement>(req->get_operation());

        memory_requirement *mreq =
            cast<memory_requirement>(req->get_operation());

        if(mreq->is_buffer_requirement()) {
          cast<buffer_memory_requirement>(mreq)
              ->get_data_region()
              ->get_users()
              .release_dead_users();
        }
        else
          assert(false && "Non-buffer requirements are unsupported");
      }

      // Go!!!
      _scheduler.submit(&new_dag);
    }
  });
}

void dag_manager::flush_sync()
{
  this->flush_async();
  _worker.wait();
}

void dag_manager::wait()
{
  this->_submitted_ops.wait_for_all();
}

void dag_manager::register_submitted_ops(const dag_interpreter &interpreter)
{
  this->_submitted_ops.update_with_submission(interpreter);
}

void dag_manager::trigger_flush_opportunity()
{
  if(builder()->get_current_dag_size() > max_cached_dag_nodes)
    flush_async();
}

}
}