/*
 * CppCrail: Native Crail
 *
 * Author: Patrick Stuedi  <pstuedi@gmail.com>
 *
 * Copyright (C) 2015-2018
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "crail/client/namenode/remove_request.h"

RemoveRequest::RemoveRequest(Filename &name, bool recursive)
    : NamenodeRequest(static_cast<short>(RpcCommand::Removefile),
                      static_cast<short>(RequestType::Removefile)),
      filename_(name) {
  this->filename_ = std::move(name);
  this->recursive_ = recursive;
}
RemoveRequest::~RemoveRequest() {}

int RemoveRequest::Write(ByteBuffer &buffer) {
  NamenodeRequest::Write(buffer);

  filename_.Write(buffer);
  int recursive = this->recursive_ ? 1 : 0;
  buffer.PutInt(recursive);

  return 0;
}
