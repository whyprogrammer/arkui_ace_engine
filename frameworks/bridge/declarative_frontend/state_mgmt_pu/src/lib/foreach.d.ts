/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
 */

// regular ForEach ===========================

declare class ForEach {
  static create(): void;
  static pop(): void;
  static getIdArray(elmtId: number, emptyArray: string[]): boolean;
  static setIdArray(elmtId: number, newIdArray: string[]): void;
  static createNewChildStart(id: string, parentView: NativeView): void;
  static createNewChildFinish(id: string, parentView: NativeView): void;
}


// LazyForEach ===========================

declare interface DataChangeListener {
  onDataReloaded(): void;
  onDataAdded(index: number): void;
  onDataMoved(from: number, to: number): void;
  onDataDeleted(index: number): void;
  onDataChanged(index: number): void;
}
declare interface IDataSource {
  totalCount(): number;
  getData(index: number): any;
  registerDataChangeListener(listener: DataChangeListener): void;
  unregisterDataChangeListener(listener: DataChangeListener): void;
}

declare class LazyForEach  {
  static create(id: string, parent: View, dataSource: IDataSource, builder: (item: any) => void, idfunc?: (item: any) => string); void;
  static pop() : void;
}

