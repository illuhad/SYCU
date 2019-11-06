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

#include "CL/sycl/detail/scheduling/hints.hpp"

namespace cl {
namespace sycl {
namespace detail {

execution_hint::execution_hint(execution_hint_type type)
: _type{type}
{}

execution_hint_type execution_hint::get_hint_type() const
{
  return _type;
}

execution_hint::~execution_hint(){}

namespace hints {

bind_to_device::bind_to_device(device_id d)
: _dev{d}, execution_hint{execution_hint_type::bind_to_device}
{}

device_id bind_to_device::get_device_id() const
{ return _dev; }


explicit_require::explicit_require(dag_node_ptr node)
: _dag_node{node}, 
  execution_hint{execution_hint_type::explicit_require}
{}

dag_node_ptr explicit_require::get_requirement() const
{
  return _dag_node;
}


dag_expander_annotation::dag_expander_annotation()
: execution_hint{execution_hint_type::dag_expander_annotation}
{}

void dag_expander_annotation::set_optimized_away()
{
  _replacement_operation = nullptr;
  _forwarding_target = nullptr;
}

void dag_expander_annotation::set_replacement_operation(std::unique_ptr<operation> op)
{
  _replacement_operation = std::move(op);
  _forwarding_target = nullptr;
}

void dag_expander_annotation::set_forward_to_node(dag_node_ptr forward_to_node)
{
  _replacement_operation = nullptr;
  _forwarding_target = forward_to_node;
}

bool dag_expander_annotation::is_optimized_away() const
{
  return _replacement_operation == nullptr && _forwarding_target == nullptr;
}

bool dag_expander_annotation::is_operation_replaced() const
{
  return _replacement_operation != nullptr;
}

bool dag_expander_annotation::is_node_forwarded() const
{
  return _forwarding_target != nullptr;
}

operation* dag_expander_annotation::get_replacement_operation() const
{
  return _replacement_operation.get();
}

dag_node_ptr dag_expander_annotation::get_forwarding_target() const
{
  return _forwarding_target;
}

} // hints



void execution_hints::add_hint(execution_hint_ptr hint)
{
  _hints.push_back(hint);
}

void execution_hints::overwrite_with(const execution_hints& other)
{
  for(const auto& hint : other._hints)
  {
    execution_hint_type type = hint->get_hint_type();
    auto it = std::find_if(_hints.begin(),_hints.end(),
      [type](execution_hint_ptr h){
      return type == h->get_hint_type();
    });

    if(it != _hints.end())
      *it = hint;
  }
}

bool execution_hints::has_hint(execution_hint_type type) const
{
  return get_hint(type) != nullptr;
}

execution_hint* execution_hints::get_hint(execution_hint_type type) const
{
  for(const auto& hint : _hints)
    if(hint->get_hint_type() == type)
      return hint;
  return nullptr;
}


}
}
}