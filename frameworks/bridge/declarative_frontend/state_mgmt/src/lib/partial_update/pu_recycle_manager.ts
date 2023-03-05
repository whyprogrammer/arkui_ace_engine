/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
 *
 *  * RecycleManager - Recycle cache manager
 *
* all definitions in this file are framework internal
*/

/**
 * Manage the js cache of all recycled node
 */
class RecycleManager {
  cachedRecycleNodes: Map<string, Array<ViewPU>>
  static instance_: RecycleManager

  constructor() {
    this.cachedRecycleNodes = new Map();
  }

  static GetInstance(): RecycleManager {
    if (!RecycleManager.instance_) {
      RecycleManager.instance_ = new RecycleManager();
    }
    return RecycleManager.instance_;
  }

  add(name: string, node: ViewPU): void {
    if (!this.cachedRecycleNodes.get(name)) {
      this.cachedRecycleNodes.set(name, new Array());
    }
    this.cachedRecycleNodes.get(name)?.push(node);
  }

  get(name: string): ViewPU {
    let node = this.cachedRecycleNodes.get(name)?.pop();
    return node
  }
}