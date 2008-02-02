
// *-----------------------------------------------------------------
//
// Forward Declares
//
// *-----------------------------------------------------------------

// ***TODO*** 这个文件删掉, 前置声明的目的就是减少依赖
// 用了这个文件后, 新加一个类都要到这里来声明一下
// 结果所以包含了这个文件的地方都会重新编译, 得不偿失

#pragma once

// 环形缓冲区
class CircularBuffer;

// 字节缓冲区
class ByteBuffer;

// 状态机
class SocketState;
class SocketService;

// 数据库
class Field;
class QueryResult;
class Database;

// WorldThread相关
class WorldPacket;

// Entity相关
class Entity;
class EntityManager;

// Component相关
class ComponentBase;
class PropertySetComp;
class UnitInterfComp;
class DataIOComp;
