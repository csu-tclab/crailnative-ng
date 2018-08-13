/*
 * Copyright (C) 2015-2018, IBM Corporation
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

#ifndef NARPC_STORAGE_CLIENT_H
#define NARPC_STORAGE_CLIENT_H

#include "narpc/rpc_client.h"
#include "storage/storage_client.h"
#include "storage/storage_response.h"

class NarpcStorageClient : public RpcClient, public StorageClient {
public:
  NarpcStorageClient();
  virtual ~NarpcStorageClient();

  static const bool kNodelay = true;

  int Connect(int address, int port) {
    return RpcClient::Connect(address, port);
  }
  int Close() { return RpcClient::Close(); }
  shared_ptr<StorageResponse> WriteData(int key, long long address,
                                        shared_ptr<ByteBuffer> buf);
  shared_ptr<StorageResponse> ReadData(int key, long long address,
                                       shared_ptr<ByteBuffer> buf);
};

#endif /* NARPC_STORAGE_CLIENT_H */
