/************************************************************************
*
* CppSharp
* Licensed under the MIT license.
*
************************************************************************/

#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <string>

#define CS_FLAGS

#if defined(_MSC_VER) && !defined(__clang__)
#define CS_API __declspec(dllexport)
#else
#define CS_API 
#endif

namespace CppSharp { namespace CppParser {

// Types

struct CS_API Type
{

};

struct CS_API TypeQualifiers
{
    bool IsConst;
    bool IsVolatile;
    bool IsRestrict;
};

struct CS_API QualifiedType
{
    Type* Type;
    TypeQualifiers Qualifiers;
};

struct Declaration;

struct CS_API TagType : public Type
{
    Declaration* Declaration;
};

struct CS_API ArrayType : public Type
{
    enum class ArraySize
    {
        Constant,
        Variable,
        Dependent,
        Incomplete
    };

    QualifiedType QualifiedType;
    ArraySize SizeType;
    long Size;
};

struct Parameter;

enum class CallingConvention
{
    Default,
    C,
    StdCall,
    ThisCall,
    FastCall,
    Unknown
};

struct CS_API FunctionType : public Type
{
    QualifiedType ReturnType;
    std::vector<Parameter*> Parameters;
    CallingConvention CallingConvention;
};

struct CS_API PointerType : public Type
{
    enum struct TypeModifier
    {
        Value,
        Pointer,
        LVReference,
        RVReference
    };

    QualifiedType QualifiedPointee;
    TypeModifier Modifier;
};

struct CS_API MemberPointerType : public Type
{
    QualifiedType Pointee;
};

struct TypedefDecl;

struct CS_API TypedefType : public Type
{
    TypedefDecl* Declaration;
};

struct CS_API DecayedType : public Type
{
    QualifiedType Decayed;
    QualifiedType Original;
    QualifiedType Pointee;
};

struct CS_API TemplateArgument
{
    enum struct ArgumentKind
    {
        Type,
        Declaration,
        NullPtr,
        Integral,
        Template,
        TemplateExpansion,
        Expression,
        Pack
    };

    ArgumentKind Kind;
    QualifiedType Type;
    Declaration* Declaration;
    long Integral;
};

struct Template;

struct CS_API TemplateSpecializationType : public Type
{
    std::vector<TemplateArgument> Arguments;
    Template* Template;
    Type* Desugared;
};

struct CS_API TemplateParameter
{
    std::string Name;
};

struct CS_API TemplateParameterType : public Type
{
    TemplateParameter Parameter;
};

struct CS_API TemplateParameterSubstitutionType : public Type
{
    QualifiedType Replacement;
};

struct Class;

struct CS_API InjectedClassNameType : public Type
{
    TemplateSpecializationType TemplateSpecialization;
    Class* Class;
};

struct CS_API DependentNameType : public Type
{

};

enum struct PrimitiveType
{
    Null,
    Void,
    Bool,
    WideChar,
    Int8,
    Char = Int8,
    UInt8,
    UChar = UInt8,
    Int16,
    UInt16,
    Int32,
    UInt32,
    Int64,
    UInt64,
    Float,
    Double,
    IntPtr
};

struct CS_API BuiltinType : public Type
{
    PrimitiveType Type;
};

// Comments

enum struct RawCommentKind
{
    Invalid,
    OrdinaryBCPL,
    OrdinaryC,
    BCPLSlash,
    BCPLExcl,
    JavaDoc,
    Qt,
    Merged
};

struct FullComment;

struct CS_API RawComment
{
    RawCommentKind Kind;
    std::string Text;
    std::string BriefText;
    FullComment* FullComment;
};

// Class layouts

enum struct VTableComponentKind
{
    VCallOffset,
    VBaseOffset,
    OffsetToTop,
    RTTI,
    FunctionPointer,
    CompleteDtorPointer,
    DeletingDtorPointer,
    UnusedFunctionPointer,
};

struct CS_API VTableComponent
{
    VTableComponentKind Kind;
    unsigned Offset;
    Declaration* Declaration;
};

struct CS_API VTableLayout
{
    std::vector<VTableComponent> Components;
};

struct CS_API VFTableInfo
{
    uint64_t VBTableIndex;
    uint32_t VFPtrOffset;
    uint32_t VFPtrFullOffset;
    VTableLayout Layout;
};

enum struct CppAbi
{
    Itanium,
    Microsoft,
    ARM
};

struct CS_API ClassLayout
{
    CppAbi ABI;
    std::vector<VFTableInfo> VFTables;
    VTableLayout Layout;
    bool HasOwnVFPtr;
    long VBPtrOffset;
    int Alignment;
    int Size;
    int DataSize;
};

// Declarations

enum struct MacroLocation
{
    Unknown,
    ClassHead,
    ClassBody,
    FunctionHead,
    FunctionParameters,
    FunctionBody,
};

enum struct AccessSpecifier
{
    Private,
    Protected,
    Public
};

struct DeclarationContext;
struct PreprocessedEntity;

struct CS_API Declaration
{
    AccessSpecifier Access;
    DeclarationContext* _Namespace;
    std::string Name;
    RawComment* Comment;
    std::string DebugText;
    bool IsIncomplete;
    bool IsDependent;
    Declaration* CompleteDeclaration;
    unsigned DefinitionOrder;
    std::vector<PreprocessedEntity*> PreprocessedEntities;
    void* OriginalPtr;
};

struct Class;
struct Enumeration;
struct Function;
struct TypedefDecl;
struct Namespace;
struct Template;
struct Variable;

struct CS_API DeclarationContext : public Declaration
{
    Declaration* FindAnonymous(uint64_t key);

    CppSharp::CppParser::Namespace* FindNamespace(const std::string& Name);
    CppSharp::CppParser::Namespace* FindNamespace(const std::vector<std::string>&);
    CppSharp::CppParser::Namespace* FindCreateNamespace(const std::string& Name);

    Class* CreateClass(std::string Name, bool IsComplete);
    Class* FindClass(const std::string& Name);
    Class* FindClass(const std::string& Name, bool IsComplete,
        bool Create = false);

    Enumeration* FindEnum(const std::string& Name, bool Create = false);

    Function* FindFunction(const std::string& Name, bool Create = false);

    TypedefDecl* FindTypedef(const std::string& Name, bool Create = false);

    std::vector<CppSharp::CppParser::Namespace*> Namespaces;
    std::vector<Enumeration*> Enums;
    std::vector<Function*> Functions;
    std::vector<Class*> Classes;
    std::vector<Template*> Templates;
    std::vector<TypedefDecl*> Typedefs;
    std::vector<Variable*> Variables;
    std::map<uint64_t, Declaration*> Anonymous;
};

struct CS_API TypedefDecl : public Declaration
{
    QualifiedType QualifiedType;
};

struct CS_API Parameter : public Declaration
{
    Parameter() : IsIndirect(false) {}

    QualifiedType QualifiedType;
    bool IsIndirect;
    bool HasDefaultValue;
};

enum struct CXXMethodKind
{
    Normal,
    Constructor,
    Destructor,
    Conversion,
    Operator,
    UsingDirective
};

enum struct CXXOperatorKind
{
    None,
    New,
    Delete,
    Array_New,
    Array_Delete,
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Caret,
    Amp,
    Pipe,
    Tilde,
    Exclaim,
    Equal,
    Less,
    Greater,
    PlusEqual,
    MinusEqual,
    StarEqual,
    SlashEqual,
    PercentEqual,
    CaretEqual,
    AmpEqual,
    PipeEqual,
    LessLess,
    GreaterGreater,
    LessLessEqual,
    GreaterGreaterEqual,
    EqualEqual,
    ExclaimEqual,
    LessEqual,
    GreaterEqual,
    AmpAmp,
    PipePipe,
    PlusPlus,
    MinusMinus,
    Comma,
    ArrowStar,
    Arrow,
    Call,
    Subscript,
    Conditional
};

struct CS_API Function : public Declaration
{
    Function() : IsReturnIndirect(false) {}

    QualifiedType ReturnType;
    bool IsReturnIndirect;

    bool IsVariadic;
    bool IsInline;
    bool IsPure;
    bool IsDeleted;
    CXXOperatorKind OperatorKind;
    std::string Mangled;
    std::string Signature;
    CallingConvention CallingConvention;
    std::vector<Parameter*> Parameters;
};

struct AccessSpecifierDecl;

struct CS_API Method : public Function
{
    AccessSpecifierDecl* AccessDecl;

    bool IsVirtual;
    bool IsStatic;
    bool IsConst;
    bool IsImplicit;
    bool IsOverride;

    CXXMethodKind Kind;

    bool IsDefaultConstructor;
    bool IsCopyConstructor;
    bool IsMoveConstructor;
};

struct CS_API Enumeration : public Declaration
{
    struct CS_API Item : public Declaration
    {
        std::string Name;
        std::string Expression;
        std::string Comment;
        uint64_t Value;
    };

    enum struct CS_FLAGS EnumModifiers
    {
        Anonymous = 1 << 0,
        Scoped = 1 << 1,
        Flags  = 1 << 2,
    };

    EnumModifiers Modifiers;
    Type* Type;
    BuiltinType* BuiltinType;
    std::vector<Item> Items;
};

struct CS_API Variable : public Declaration
{
    std::string Mangled;
    QualifiedType QualifiedType;
};

struct CS_API BaseClassSpecifier
{
    AccessSpecifier Access;
    bool IsVirtual;
    Type* Type;
};

struct Class;

struct CS_API Field : public Declaration
{
    QualifiedType QualifiedType;
    AccessSpecifier Access;
    unsigned Offset;
    Class* Class;
};


struct CS_API AccessSpecifierDecl : public Declaration
{

};

struct CS_API Class : public DeclarationContext
{
    std::vector<BaseClassSpecifier*> Bases;
    std::vector<Field*> Fields;
    std::vector<Method*> Methods;
    std::vector<AccessSpecifierDecl*> Specifiers;

    bool IsPOD;
    bool IsAbstract;
    bool IsUnion;
    bool IsDynamic;
    bool IsPolymorphic;
    bool HasNonTrivialDefaultConstructor;
    bool HasNonTrivialCopyConstructor;

    ClassLayout Layout;
};

struct CS_API Template : public Declaration
{
    Declaration* TemplatedDecl;
    std::vector<TemplateParameter> Parameters;
};

struct CS_API ClassTemplate : public Template
{
};

struct CS_API ClassTemplateSpecialization : public Declaration
{
};

struct CS_API ClassTemplatePartialSpecialization : public Declaration
{
};

struct CS_API FunctionTemplate : public Template
{
};

struct CS_API Namespace : public DeclarationContext
{

};

struct CS_API PreprocessedEntity : public Declaration
{
    PreprocessedEntity() : Location(MacroLocation::Unknown) {}

    MacroLocation Location;
};

struct CS_API MacroDefinition : public PreprocessedEntity
{
    std::string Expression;
};

struct CS_API MacroExpansion : public PreprocessedEntity
{
    std::string Text;
    MacroDefinition* Definition;
};


struct CS_API TranslationUnit : public Namespace
{
    std::string FileName;
    bool IsSystemHeader;
    std::vector<Namespace*> Namespaces;
    std::vector<MacroDefinition*> Macros;
};

struct CS_API NativeLibrary
{
    std::string FileName;
    std::vector<std::string> Symbols;
};

struct CS_API Library
{
    TranslationUnit* FindOrCreateModule(const std::string& File);
    NativeLibrary* FindOrCreateLibrary(const std::string& File);

    std::vector<TranslationUnit*> TranslationUnits;
    std::vector<NativeLibrary*> Libraries;
};

} }