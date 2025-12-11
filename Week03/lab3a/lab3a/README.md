# BÀI THỰC HÀNH 3 - PHÂN TÍCH NGỮ NGHĨA (Ngày 1)

## Hướng dẫn biên dịch và chạy

Cài đặt Make hoặc CMake, thiết lập PATH trong environment variables
Build project với Make kèm các thư viện cần thiết
Lựa chọn test file tương ứng với parser

**Trên MacOS/Linux:**

```bash
make
./symtab
```

**Trên Windows:**

```bash
mingw32-make
./symtab.exe
```

Kết quả sẽ hiển thị symbol table theo cấu trúc mô tả tại phần [Kết quả mong đợi](#kết-quả-mong-đợi---ngày-1)

## Giới thiệu về Phân tích Ngữ nghĩa

Parser chỉ đảm bảo cấu trúc ngữ pháp đúng của chương trình
Cần trả lời các câu hỏi sau:
> Định danh `x` là biến hay hàm?

> Định danh `x` đã được khai báo/định nghĩa chưa?

> Nơi khai báo của `x` ở đâu?

> Kiểu dữ liệu trong biểu thức `a + b` có tương thích không?

### Chức năng của Semantic Analyzer

- **Lưu trữ và quản lý** thông tin định danh
  - Hằng số
  - Biến số
  - Kiểu dữ liệu tự định nghĩa
  - Procedure/Function

- Kiểm định các quy tắc ngữ nghĩa
  - Phạm vi hoạt động (**Scope**) của định danh
  - Tính tương thích về kiểu dữ liệu

## Kết quả mong đợi - Ngày 1

Cài đặt các hàm có đánh dấu `TODO` trong file `symtab.c`

### Bảng ký hiệu (Symbol Table)

Với chương trình KPL mẫu:

```txt
PROGRAM test;

CONST c = 100;
TYPE t = Integer;
VAR v : t;

FUNCTION f(x : t) : t;
VAR y : t;
BEGIN
    y := x + 1;
    f := y;

END;

BEGIN
    v := 1;
    WriteLn (f(v));
END.
```

Bảng ký hiệu `symtab` tương ứng:

```txt
test : PRG
|
|-- c : CST = 100
|-- t : TY = INT
|-- v : VAR : INT
|-- f : FN: INT -> INT
    |
    |-- x :  PAR : INT
    |-- y :  VAR : INT
```

#### Cấu trúc dữ liệu

```c
struct SymTab_ {
    // Object của chương trình chính
    Object* program;

    // Con trỏ đến scope đang hoạt động
    Scope* currentScope;

    // Danh sách các object toàn cục
    // (WRITEI, WRITEC, WRITELN, READI, READC)
    ObjectNode *globalObjectList;
};
```

Thành phần của Scope:

```c
struct Scope_ {
    // Các objects trong phạm vi này
    ObjectNode *objList;

    // Object sở hữu scope này
    // (Func/Proc/Program)
    Object *owner;

    // Con trỏ đến scope bên ngoài
    // (dùng khi thoát khỏi block hiện tại)
    struct Scope_ *outer;
};
```

#### Thao tác với Scope

Khi vào/ra một function/procedure, cần cập nhật `currentScope`

```c
void enterBlock(Scope* scope);
void exitBlock(void);
```

Thêm một object vào scope hiện tại

```c
void declareObject(Object* obj);
```

### Đối tượng (Object)

Thuộc tính được truy cập qua con trỏ như `obj->typeAttrs`

```c
struct ConstantAttributes_ {
    ConstantValue* value;
};

struct VariableAttributes_ {
    Type *type;
    // Scope chứa biến (dùng cho giai đoạn sinh code)
    struct Scope_ *scope;
};

struct TypeAttributes_ {
    Type *actualType;
};

struct ParameterAttributes_ {
    // Phân loại: tham trị hay tham chiếu
    enum ParamKind kind;
    Type* type;
    struct Object_ *function;
};

struct ProcedureAttributes_ {
    struct ObjectNode_ *paramList;
    struct Scope_* scope;
};

struct FunctionAttributes_ {
    struct ObjectNode_ *paramList;
    Type* returnType;
    struct Scope_ *scope;
};

struct ProgramAttributes_ {
    struct Scope_ *scope;
};
```

### Khởi tạo object

Mỗi object cần gắn:

- Loại (Kind): `OBJ_CONSTANT`/`OBJ_TYPE`/`OBJ_VARIABLE`/...
- Thuộc tính: `constAttrs`/`typeAttrs`/`varAttrs`/...
- Phạm vi: Áp dụng cho Variable, Func/Proc
- Danh sách tham số: Dành cho Func/Proc (Parameter cũng là object)
→ Parameter cần thiết lập `owner`

```c
Object* createConstantObject(char *name);
Object* createTypeObject(char *name);
Object* createVariableObject(char *name);
Object* createParameterObject(
    char *name,
    enum ParamKind kind,
    Object* owner
);
```

```c
Object* createFunctionObject(char *name);
Object* createProcedureObject(char *name);
Object* createProgramObject(char *name);
```

### Giải phóng bộ nhớ

Các hàm free được tổng hợp trong `cleanSymTab`

```c
void cleanSymTab(void) {
  freeObject(symtab->program);
  freeObjectList(symtab->globalObjectList);
  free(symtab);
  freeType(intType);
  freeType(charType);
}
```

Các hàm phụ trợ trong `freeObject()`:

```c
// Giải phóng danh sách object references
void freeReferenceList(ObjectNode* objList)
// Giải phóng scope
void freeScope(Scope* scope)
```