open System
open System.Threading

(*-------------------- Zmienne globalne--------------------*)

let display_memory: bool = true
let mutable file_path = ""
let mutable loop:bool = true
#if VM_ADDRESSING_32
let AddressSize = 4u 
#else
let AddressSize = 2us
#endif
let DIV_BY_ZERO_EXC = 1
let OUT_OF_BOUNDS_EXC = 2
let OUT_OF_BOUNDS_FLG = 2us
(*-------------------- Dziedziny --------------------*)
type BasicTypes = 
        | OneByte of byte
        | TwoBytes of uint16
        | FourBytes of uint32
        | EightBytes of uint64

type OneByte = uint8
type TwoBytes = uint16
type FourBytes = uint32

#if VM_ADDRESSING_32
type Address = uint32
#else
type Address = uint16
#endif

#if VM_ADDRESSING_32
[<System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.AggressiveInlining)>]
let (.+.) (a:Address) (offs:int) = a + (uint32 offs)
#else
[<System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.AggressiveInlining)>]
let (.+.) (a:Address) (offs:int) = a + (uint16 offs)
#endif

type Storage = byte array
type Memory = (Address*Storage) -> byte
type Stack = Address list
type Flags = uint16

type ProtEntry = (Address * Address * Address * Address)
type ProtStack = ProtEntry list
type ExcObj = Address * int
type ExcState = ProtStack * ExcObj

type State = (Storage * Storage * Stack * Stack * Address * Address * ExcState * Flags)

exception Error1 of string

type ValueType =
    | BOOL of bool
    | BYTE of byte
    | WORD of uint16
    | DWORD of uint32
    | LWORD of uint64
    | SINT of sbyte
    | INT of int16 
    | DINT of int32    
    | LINT of int64
    | REAL of float32
    | LREAL of float
    | TIME of uint32


(*-------------------- Funkcje odczytu i zapisu pamięci --------------------*)

// odczyt 1 bajtu spod podanego adresu w danej pamięci
let Get1BMem (address:Address, mem:Storage) : byte = 
              mem.[int <| address]

// odczyt 2 bajtów spod podanego adresu w danej pamięci
let Get2BMem (address:Address, mem:Storage) : uint16 = 
             let byteArray = [| 
                                mem.[int <| address]
                                mem.[int <| (address .+. 1)]
                             |]
             BitConverter.ToUInt16(byteArray,0)

// odczyt 4 bajtów spod podanego adresu w danej pamięci

let Get4BMem (address:Address, mem:Storage) : uint32 =
             let byteArray = [|
                                let mutable iterator = address
                                while (iterator < address .+. 4) do 
                                       yield mem.[int <| iterator]
                                       iterator <- iterator .+. 1
                             |]
             BitConverter.ToUInt32(byteArray,0)

// odczyt 8 bajtów spod podanego adresu w danej pamięci

let Get8BMem (address:Address, mem:Storage) : uint64 =
             let byteArray = [|
                                let mutable iterator = address
                                while (iterator < address .+. 8) do 
                                       yield mem.[int <| iterator]
                                       iterator <- iterator .+. 1
                             |]
             BitConverter.ToUInt64(byteArray,0)



// zmiana wartości 1 bajtu pamięci pod zadanym adresem podaną wartością        
let Upd1BMem (address:Address, mem:Storage, value:byte) : Storage = 
             let new_mem = Array.copy mem
             new_mem.[int <| address] <- value
             new_mem 

// zmiana wartości 2 bajtów pamięci pod zadanym adresem podaną wartością
let Upd2BMem (address:Address, mem:Storage, value:uint16) : Storage =
             let new_mem = Array.copy mem
             let byteArray = BitConverter.GetBytes(value)
             let mutable iterator = address
             for b in byteArray do
                    new_mem.[int <| iterator] <- b
                    iterator <- iterator .+. 1
             new_mem

// zmiana wartości 4 bajtów pamięci pod zadanym adresem podaną wartością
let Upd4BMem (address:Address, mem:Storage, value:uint32) : Storage =
             let new_mem = Array.copy mem
             let byteArray = BitConverter.GetBytes(value)
             let mutable iterator = address
             for b in byteArray do
                    new_mem.[int <| iterator] <- b
                    iterator <- iterator .+. 1
             new_mem

// zmiana wartości 8 bajtów pamięci pod zadanym adresem podaną wartością
let Upd8BMem (address:Address, mem:Storage, value:uint64) : Storage =
             let new_mem = Array.copy mem
             let byteArray = BitConverter.GetBytes(value)
             let mutable iterator = address
             for b in byteArray do
                    new_mem.[int <| iterator] <- b
                    iterator <- iterator .+. 1
             new_mem

(*--------------------   Funkcja kopiująca pamięć  --------------------*)

//kopiowanie określonej ilości bajtów z pamięci mem2 od adresu adres2 do pamięci mem1 od adresu adres1
let MemMove (address1:Address, mem1:Storage, address2:Address , mem2:Storage, count:uint16) :Storage=
                let new_mem = Array.copy mem1
                let copiedByteArray = [|
                                         let mutable iterator = address2
                                         let _tmp_count = int32 count
                                         while (iterator < address2 .+. _tmp_count) do
                                               yield mem2.[int <| iterator]
                                               iterator <- iterator .+. 1
                                       |]
                let mutable iterator2 = address1
                for b in copiedByteArray do
                      new_mem.[int <| iterator2] <- b
                      iterator2 <- iterator2 .+. 1
                new_mem

(*--------------------   Funkcja wypełniająca pamięć  --------------------*)

let MemFill (address:Address, mem:Storage, value: byte, count: uint16) : Storage=
            let new_mem = Array.copy mem
            let mutable iterator = 0
            let _tmp_count = int32 count
            while (iterator < _tmp_count) do
                  new_mem.[int <| (address .+. iterator)] <- value
                  iterator <- iterator + LanguagePrimitives.GenericOne
            new_mem  

(*-------------------- Funkcje operujące na stosie --------------------*)

//Umieszczenie wartości na stosie
let Push (stack:Stack, address:Address) : Stack = 
    match stack with
    |[] -> [address]
    |_ -> stack @ [address]

//Pobranie wartości ze stosu
let Pop (stack:Stack) = 
    match stack with
    |[] ->  printfn "Stos pusty"; (None, [])
    |_  ->  let new_stack = List.take <| List.length stack - 1 <| stack
            (Some <| List.last stack, new_stack)


(*-------------------- Funkcja pobierająca wartość będącą adresem --------------------*)

#if VM_ADDRESSING_32
let GetAddress (address:Address,mem:Storage) : Address = 
            let byteArray = [| 
                                mem.[int <| address]
                                mem.[int <| (address .+. 1)]
                                mem.[int <| (address .+. 2)]
                                mem.[int <| (address .+. 3)]
                            |]
            BitConverter.ToUInt32(byteArray, 0)
#else
let GetAddress (address:Address,mem:Storage) : Address = 
            let byteArray = [| 
                                mem.[int <| address]
                                mem.[int <| (address .+. 1)]
                            |]
            BitConverter.ToUInt16(byteArray, 0)
#endif
//_tmp_count
(*-------------------- Funkcje ustawiające i czyszczące bity --------------------*)

let ClearFlag (flag:uint16,value:uint16) : uint16 = 
            flag &&& (~~~value)

let SetFlag (flag:uint16, value:uint16) : uint16 =
            flag ||| value

(*-------------------- Funkcje do konwersji wartości --------------------*)

let ByteToWord (value : byte) : uint16 = 
           Convert.ToUInt16(value)


let WordToByte (value:uint16) : byte =
           Convert.ToByte(value)

let ByteToInt (value : byte) : uint32 = 
           Convert.ToUInt32(value)

(*-------------------- Interpretacja wartości --------------------*)

let BoolOf (a:byte)=
          match a with
          |0uy -> BOOL(false)
          |_ -> BOOL(true)

let FromBool (a:ValueType) : byte =
          match a with
          |BOOL a -> Convert.ToByte(a)
          |_ -> failwith "Błędne dane"

let ByteOf (a:byte) =
          BYTE(a)

let FromByte (a:ValueType) : byte =
          match a with
          |BYTE a -> a
          |_ -> failwith "Błędne dane"
          
let WordOf (a:uint16) =
          WORD(a)

let FromWord (a:ValueType) : uint16 =
          match a with 
          |WORD a -> a
          |_ -> failwith "Błędne dane"

let DWordOf (a:uint32) =
          DWORD(a)

let FromDWord (a:ValueType) : uint32 =
          match a with 
          |DWORD a -> a
          |_ -> failwith "Błędne dane"

let LWordOf (a:uint64) =
          LWORD(a)

let FromLWord (a:ValueType) : uint64 =
          match a with 
          |LWORD a -> a
          |_ -> failwith "Błędne dane"

let SIntOf (a:byte) =
          let value = sbyte <| a
          SINT(value)

let FromSInt (a:ValueType) : byte =
          match a with 
          |SINT a -> byte <| a
          |_ -> failwith "Błędne dane"

let IntOf (a:uint16) =
          let byteArray = BitConverter.GetBytes(a)
          let value = BitConverter.ToInt16(byteArray,0)
          INT(value)

let FromInt (a:ValueType) : uint16 =
          match a with 
          |INT a -> let byteArray = BitConverter.GetBytes(a)
                    let value = BitConverter.ToUInt16(byteArray,0)
                    value
          |_ -> failwith "Błędne dane"

let DIntOf (a:uint32)=
          let byteArray = BitConverter.GetBytes(a)
          let value = BitConverter.ToInt32(byteArray,0)
          DINT(value)

let FromDInt (a:ValueType) : uint32 =
          match a with
          |DINT a -> let byteArray = BitConverter.GetBytes(a)
                     let value = BitConverter.ToUInt32(byteArray,0)
                     value
          |_ -> failwith "Błędne dane"

let LIntOf (a:uint64)=
          let byteArray = BitConverter.GetBytes(a)
          let value = BitConverter.ToInt64(byteArray,0)
          LINT(value)

let FromLInt (a:ValueType) : uint64 =
          match a with
          |LINT a -> let byteArray = BitConverter.GetBytes(a)
                     let value = BitConverter.ToUInt64(byteArray,0)
                     value
          |_ -> failwith "Błędne dane"

let RealOf (a:uint32)=
          let byteArray = BitConverter.GetBytes(a)
          let value = BitConverter.ToSingle(byteArray,0)
          REAL(value)

let FromReal (a:ValueType) : uint32 =
          match a with
          |REAL a -> let byteArray = BitConverter.GetBytes(a)
                     let value = BitConverter.ToUInt32(byteArray,0)
                     value
          |_ -> failwith "Błędne dane"

let LRealOf (a:uint64)=
          let byteArray = BitConverter.GetBytes(a)
          let value = BitConverter.ToDouble(byteArray,0)
          LREAL(value)

let FromLReal (a:ValueType) : uint64 =
          match a with
          |LREAL a -> let byteArray = BitConverter.GetBytes(a)
                      let value = BitConverter.ToUInt64(byteArray,0)
                      value
          |_ -> failwith "Błędne dane"


let TimeOf (a:uint32)=
          let byteArray = BitConverter.GetBytes(a)
          let value = BitConverter.ToUInt32(byteArray,0)
          TIME(value)

let FromTime (a:ValueType) : uint32 =
          match a with
          |TIME a -> let byteArray = BitConverter.GetBytes(a)
                     let value = BitConverter.ToUInt32(byteArray,0)
                     value
          |_ -> failwith "Błędne dane"


(*-------------------- Procedury systemowe --------------------*)


let RAISE(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state 
     printfn "An exception is raised"
     let clbl = GetAddress(cr,cm)
     (cm,dm,cs,ds,clbl,dr,ex,flg)

let JMP(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state 
     let clbl = GetAddress(cr,cm)
     (cm,dm,cs,ds,clbl,dr,ex,flg)

let JNZ(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state 
     let cnd = GetAddress(cr,cm)
     let cndaddr = dr + cnd
     let cr1 = cr + AddressSize
     let clbl = GetAddress(cr1,cm)
     let cr2 = cr1 + AddressSize
     let ctl = BoolOf(Get1BMem(cndaddr,dm))
     let state1:State = match ctl with
                        |BOOL true  -> (cm,dm,cs,ds,clbl,dr,ex,flg)
                        |BOOL false -> (cm,dm,cs,ds,cr2,dr,ex,flg)
                        |_          -> failwith "Błędne dane"
     state1

let JZ(state:State) : State =
    let (cm,dm,cs,ds,cr,dr,ex,flg) = state
    let cnd = GetAddress(cr,cm)
    let cndaddr = dr + cnd
    let cr1 = cr + AddressSize
    let clbl = GetAddress(cr1,cm)
    let cr2 = cr1 + AddressSize
    let ctl = BoolOf(Get1BMem(cndaddr,dm))
    let state1:State = match ctl with
                       |BOOL true  -> (cm,dm,cs,ds,cr2,dr,ex,flg)
                       |BOOL false -> (cm,dm,cs,ds,clbl,dr,ex,flg)
                       |_          -> failwith "Błędne dane"
    state1

let JR(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let clbl = GetAddress(cr,cm)
     let cr1 = cr + AddressSize
     let clbladdr = cr1 + clbl
     (cm,dm,cs,ds,clbladdr,dr,ex,flg)
     
let JRN(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let cnd = GetAddress(cr,cm)
     let cndaddr = dr + cnd
     let cr1 = cr + AddressSize
     let clbl = GetAddress(cr1,cm)
     let cr2 = cr1 + AddressSize
     let clbladdr = cr2 + clbl
     let ctl = BoolOf(Get1BMem(cndaddr,dm))
     let state1:State = match ctl with
                        |BOOL true  -> (cm,dm,cs,ds,clbladdr,dr,ex,flg)
                        |BOOL false -> (cm,dm,cs,ds,cr2,dr,ex,flg)
                        |_          -> failwith "Błędne dane"
     state1 

let JRZ(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let cnd = GetAddress(cr,cm)
     let cndaddr = dr + cnd
     let cr1 = cr + AddressSize
     let clbl = GetAddress(cr1,cm)
     let cr2 = cr1 + AddressSize
     let clbladdr = cr2 + clbl
     let ctl = BoolOf(Get1BMem(cndaddr,dm))
     let state1:State = match ctl with
                        |BOOL true  -> (cm,dm,cs,ds,cr2,dr,ex,flg)
                        |BOOL false -> (cm,dm,cs,ds,clbladdr,dr,ex,flg)
                        |_          -> failwith "Błędne dane"
     state1

let RETURN(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state 
     let (clbl1, cstk) = Pop(cs)
     let (dr1, dstk) = Pop(ds)
     (cm,dm,cstk,dstk,clbl1.Value,dr1.Value,ex,flg)

let MCD(state : State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state 
     let dst = GetAddress(cr,cm)
     let cr1 = cr + AddressSize
     let dstaddr = dr + dst
     let size = ByteOf(Get1BMem(cr1,cm))
     let cr2 = cr1 .+. 1
     let um = MemMove(dstaddr,dm,cr2,cm,ByteToWord( match size with 
                                                    |BYTE a -> a
                                                    |_ -> failwith "Błędne dane"))

     let state1:State = (cm,um,cs,ds,cr2 .+. (int32 <| ByteToInt( match size with 
                                                                  |BYTE a -> a
                                                                  |_      -> failwith "Błędne dane") ),dr,ex,flg) 
     state1

let CALB(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state 
     let inst = GetAddress(cr,cm)
     let instaddr = dr + inst
     let cr1 = cr + AddressSize
     let clbl = GetAddress(cr1,cm)
     let cr2 = cr1 + AddressSize
     (cm,dm, Push(cs,cr2), Push(ds,dr), clbl, instaddr, ex, flg)

let MMV1(state:State, count:uint16) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let dst = GetAddress(cr,cm)
     let dstaddr = dr + dst
     let cr1 = cr + AddressSize
     let src = GetAddress(cr1,cm)
     let cr2 = cr1 + AddressSize
     let um = MemMove(dstaddr,dm,src,dm,count)
     (cm,um,cs,ds,cr2,dr,ex,flg)

let MMV2(state:State, count:uint16) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let dst = GetAddress(cr,cm)
     let cr1 = cr + AddressSize
     let src = GetAddress(cr1,cm)
     let srcaddr = dr + src
     let cr2 = cr1 + AddressSize
     let um = MemMove(dst,dm,srcaddr,dm,count)
     (cm,um,cs,ds,cr2,dr,ex,flg)

let TRML(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let clbl = GetAddress(cr,cm)
     loop <- false
     (cm,dm,cs,ds,clbl,dr,ex,flg)

let NOP(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     (cm,dm,cs,ds,cr,dr,ex,flg)

let MEMCP(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let dst = GetAddress(cr,cm)
     let dstaddr = dr + dst
     let cr1 = cr + AddressSize
     let src = GetAddress(cr1,cm)
     let srcaddr = dr + src
     let cr2 = cr1 + AddressSize
     let count = WordOf(Get2BMem(cr2,cm))
     let cr3 = cr2 .+. 2
     let um = MemMove(dstaddr,dm,srcaddr,dm, match count with
                                             |WORD a -> a
                                             |_      -> failwith "Błędne dane")
     (cm,um,cs,ds,cr3,dr,ex,flg)

let AURD(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let dst = GetAddress(cr,cm)
     let dstaddr = dr + dst
     let cr1 = cr + AddressSize
     let src = GetAddress(cr1,cm)
     let srcaddr = dr + src
     let cr2 = cr1 + AddressSize
     let idx = GetAddress(cr2,cm)
     let idxaddr = dr + idx
     let cr3 = cr2 + AddressSize
     let size = GetAddress(cr3,cm)
     let sizeaddr = dr + size
     let cr4 = cr3 + AddressSize
     let idxval = IntOf(Get2BMem(idxaddr,dm))
     let sizeval = ByteOf(Get1BMem(sizeaddr,dm))

     let sourceaddr = srcaddr .+. match sizeval,idxval with
                                  |BYTE a, INT b -> let _b = int32 b in (int32 a) * _b
                                  |_ -> failwith "Błędne dane"

     let um = MemMove(dstaddr,dm,sourceaddr,dm, match sizeval with
                                                |BYTE a -> (uint16 <|a)
                                                |_ -> failwith "Błędne dane")
     (cm,um,cs,ds,cr4,dr,ex,flg)

let AUWR(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let src = GetAddress(cr,cm)
     let srcaddr = dr + src
     let cr1 = cr + AddressSize
     let dst = GetAddress(cr1,cm)
     let dstaddr = dr + dst
     let cr2 = cr1 + AddressSize
     let idx = GetAddress(cr2,cm)
     let idxaddr = dr + idx
     let cr3 = cr2 + AddressSize
     let size = GetAddress(cr3,cm)
     let sizeaddr = dr + size
     let cr4 = cr3 + AddressSize
     let idxval = IntOf(Get2BMem(idxaddr,dm))
     let sizeval = ByteOf(Get1BMem(sizeaddr,dm))

     let destinationaddr = dstaddr .+. match sizeval,idxval with
                                       |BYTE a, INT b -> let _b = int32 b in (int32 a) * _b
                                       |_ -> failwith "Błędne dane"

     let um = MemMove(destinationaddr,dm,srcaddr,dm, match sizeval with
                                                     |BYTE a -> (uint16 <|a)
                                                     |_ -> failwith "Błędne dane")
     (cm,um,cs,ds,cr4,dr,ex,flg)

let AORD(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let dst = GetAddress(cr,cm)
     let dstaddr = dr + dst
     let cr1 = cr + AddressSize
     let src = GetAddress(cr1,cm)
     let srcaddr = dr + src
     let cr2 = cr1 + AddressSize
     let idx = GetAddress(cr2,cm)
     let idxaddr = dr + idx
     let cr3 = cr2 + AddressSize
     let idxval = IntOf(Get2BMem(idxaddr,dm))

     let sourceaddr = srcaddr .+. match idxval with
                                  |INT a -> int32 a
                                  |_ -> failwith "Błędne dane"
     
     let um = MemMove(dstaddr,dm,sourceaddr,dm,1us)
     (cm,um,cs,ds,cr3,dr,ex,flg)

let AOWR(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let src = GetAddress(cr,cm)
     let srcaddr = dr + src
     let cr1 = cr + AddressSize
     let dst = GetAddress(cr1,cm)
     let dstaddr = dr + dst
     let cr2 = cr1 + AddressSize
     let idx = GetAddress(cr2,cm)
     let idxaddr = dr + idx
     let cr3 = cr2 + AddressSize
     let idxval = IntOf(Get2BMem(idxaddr,dm))

     let destinationaddr = dstaddr .+. match idxval with
                                       |INT a -> int32 a
                                       |_ -> failwith "Błędne dane"
     
     let um = MemMove(destinationaddr,dm,srcaddr,dm,1us)
     (cm,um,cs,ds,cr3,dr,ex,flg)
     
let ATRD(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let dst = GetAddress(cr,cm)
     let dstaddr = dr + dst
     let cr1 = cr + AddressSize
     let src = GetAddress(cr1,cm)
     let srcaddr = dr + src
     let cr2 = cr1 + AddressSize
     let idx = GetAddress(cr2,cm)
     let idxaddr = dr + idx
     let cr3 = cr2 + AddressSize
     let idxval = IntOf(Get2BMem(idxaddr,dm))

     let sourceaddr = srcaddr .+. match idxval with
                                  |INT a -> 2 * (int32 a)
                                  |_     -> failwith "Błędne dane"
     
     let um = MemMove(dstaddr,dm,sourceaddr,dm,2us)
     (cm,um,cs,ds,cr3,dr,ex,flg)

let ATWR(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let src = GetAddress(cr,cm)
     let srcaddr = dr + src
     let cr1 = cr + AddressSize
     let dst = GetAddress(cr1,cm)
     let dstaddr = dr + dst
     let cr2 = cr1 + AddressSize
     let idx = GetAddress(cr2,cm)
     let idxaddr = dr + idx
     let cr3 = cr2 + AddressSize
     let idxval = IntOf(Get2BMem(idxaddr,dm))

     let destinationaddr = dstaddr .+. match idxval with
                                       |INT a -> 2 * (int32 a)
                                       |_     -> failwith "Błędne dane"
     
     let um = MemMove(destinationaddr,dm,srcaddr,dm,2us)
     (cm,um,cs,ds,cr3,dr,ex,flg)

let AFRD(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let dst = GetAddress(cr,cm)
     let dstaddr = dr + dst
     let cr1 = cr + AddressSize
     let src = GetAddress(cr1,cm)
     let srcaddr = dr + src
     let cr2 = cr1 + AddressSize
     let idx = GetAddress(cr2,cm)
     let idxaddr = dr + idx
     let cr3 = cr2 + AddressSize
     let idxval = IntOf(Get2BMem(idxaddr,dm))

     let sourceaddr = srcaddr .+. match idxval with
                                  |INT a -> 4 * (int32 a)
                                  |_ -> failwith "Błędne dane"
     
     let um = MemMove(dstaddr,dm,sourceaddr,dm,4us)
     (cm,um,cs,ds,cr3,dr,ex,flg)

let AFWR(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let src = GetAddress(cr,cm)
     let srcaddr = dr + src
     let cr1 = cr + AddressSize
     let dst = GetAddress(cr1,cm)
     let dstaddr = dr + dst
     let cr2 = cr1 + AddressSize
     let idx = GetAddress(cr2,cm)
     let idxaddr = dr + idx
     let cr3 = cr2 + AddressSize
     let idxval = IntOf(Get2BMem(idxaddr,dm))

     let destinationaddr = dstaddr .+. match idxval with
                                       |INT a -> 4 * (int32 a)
                                       |_ -> failwith "Błędne dane"
     
     let um = MemMove(destinationaddr,dm,srcaddr,dm,4us)
     (cm,um,cs,ds,cr3,dr,ex,flg)

let AQRD(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let dst = GetAddress(cr,cm)
     let dstaddr = dr + dst
     let cr1 = cr + AddressSize
     let src = GetAddress(cr1,cm)
     let srcaddr = dr + src
     let cr2 = cr1 + AddressSize
     let idx = GetAddress(cr2,cm)
     let idxaddr = dr + idx
     let cr3 = cr2 + AddressSize
     let idxval = IntOf(Get2BMem(idxaddr,dm))

     let sourceaddr = srcaddr .+. match idxval with
                                  |INT a -> 8 * (int32 a)
                                  |_ -> failwith "Błędne dane"
     
     let um = MemMove(dstaddr,dm,sourceaddr,dm,8us)
     (cm,um,cs,ds,cr3,dr,ex,flg)

let AQWR(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let src = GetAddress(cr,cm)
     let srcaddr = dr + src
     let cr1 = cr + AddressSize
     let dst = GetAddress(cr1,cm)
     let dstaddr = dr + dst
     let cr2 = cr1 + AddressSize
     let idx = GetAddress(cr2,cm)
     let idxaddr = dr + idx
     let cr3 = cr2 + AddressSize
     let idxval = IntOf(Get2BMem(idxaddr,dm))

     let destinationaddr = dstaddr .+. match idxval with
                                       |INT a -> 8 * (int32 a)
                                       |_ -> failwith "Błędne dane"
     
     let um = MemMove(destinationaddr,dm,srcaddr,dm,8us)
     (cm,um,cs,ds,cr3,dr,ex,flg)

let GARD(state:State) : State = 
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let dst = GetAddress(cr,cm)
     let dstaddr = dr + dst
     let cr1 = cr + AddressSize
     let src = GetAddress(cr1,cm)
     let cr2 = cr1 + AddressSize
     let idx = GetAddress(cr2,cm)
     let idxaddr = dr + idx
     let cr3 = cr2 + AddressSize
     let size = GetAddress(cr3,cm)
     let sizeaddr = dr + size
     let cr4 = cr3 + AddressSize
     let idxval = IntOf(Get2BMem(idxaddr,dm))
     let sizeval = ByteOf(Get1BMem(sizeaddr,dm))

     let sourceaddr = src .+. match sizeval,idxval with
                              |BYTE a, INT b -> (int32 a) * (int32 b)
                              |_ -> failwith "Błędne dane"
     
     let um = MemMove(dstaddr,dm,sourceaddr,dm, match sizeval with
                                                |BYTE a -> (uint16 <| a)
                                                |_ -> failwith "Błędne dane")
     (cm,um,cs,ds,cr4,dr,ex,flg)

let GAWR(state:State) : State = 
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let src = GetAddress(cr,cm)
     let srcaddr = dr + src
     let cr1 = cr + AddressSize
     let dst = GetAddress(cr1,cm)
     let cr2 = cr1 + AddressSize
     let idx = GetAddress(cr2,cm)
     let idxaddr = dr + idx
     let cr3 = cr2 + AddressSize
     let size = GetAddress(cr3,cm)
     let sizeaddr = dr + size
     let cr4 = cr3 + AddressSize
     let idxval = IntOf(Get2BMem(idxaddr,dm))
     let sizeval = ByteOf(Get1BMem(sizeaddr,dm))

     let destinationaddr = dst .+. match sizeval,idxval with
                                   |BYTE a, INT b -> (int32 a) * (int32 b)
                                   |_ -> failwith "Błędne dane"
     
     let um = MemMove(destinationaddr,dm,srcaddr,dm, match sizeval with
                                                     |BYTE a -> (uint16 <| a)
                                                     |_ -> failwith "Błędne dane")
     (cm,um,cs,ds,cr4,dr,ex,flg)


let DPSTO(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let dst = GetAddress(cr,cm)
     let dstaddr = dr + dst
     let cr1 = cr + AddressSize
#if VM_ADDRESSING_32
     let um = Upd4BMem(dstaddr,dm,FromDWord(DWORD(dr)))
#else
     let um = Upd2BMem(dstaddr,dm,FromWord(WORD(dr)))
#endif
     (cm,um,cs,ds,cr1,dr,ex,flg)

let DPRDL(state:State) : State = 
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let dst = GetAddress(cr,cm)
     let dstaddr = dr + dst
     let cr1 = cr + AddressSize
     let src = GetAddress(cr1,cm)
     let srcaddr = GetAddress(dr+src,dm)
     let cr2 = cr1 + AddressSize
     let count = WordOf(Get2BMem(cr2,cm))
     let cr3 = cr2 .+. 2
     let um = MemMove(dstaddr,dm,srcaddr,dm,FromWord(count))
     (cm,um,cs,ds,cr3,dr,ex,flg)

let DPWRL(state:State) : State = 
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let src = GetAddress(cr,cm)
     let srcaddr = dr + src
     let cr1 = cr + AddressSize
     let dst = GetAddress(cr1,cm)
     let dstaddr = GetAddress(dr+dst,dm)
     let cr2 = cr1 + AddressSize
     let count = WordOf(Get2BMem(cr2,cm))
     let cr3 = cr2 .+. 2
     let um = MemMove(dstaddr,dm,srcaddr,dm,FromWord(count))
     (cm,um,cs,ds,cr3,dr,ex,flg)

let FPAT(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let dst = GetAddress(cr,cm)
     let dstaddr = dr + dst
     let cr1 = cr + AddressSize
     let war = ByteOf(Get1BMem(cr1,cm))
     let cr2 = cr1 .+. 1
     let count = ByteOf(Get1BMem(cr2,cm))
     let cr3 = cr2 .+. 1
     let um = MemFill(dstaddr,dm,FromByte(war),ByteToWord(FromByte(count)))
     (cm,um,cs,ds,cr3,dr,ex,flg)

let CEAC(state:State) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let idx = GetAddress(cr,cm)
     let idxaddr = dr + idx
     let cr1 = cr + AddressSize
     let lobase = GetAddress(cr1,cm)
     let lobaseaddr = dr + lobase
     let cr2 = cr1 + AddressSize
     let hilevel = GetAddress(cr2,cm)
     let hileveladdr = dr + hilevel
     let cr3 = cr2 + AddressSize
     let idxval = IntOf(Get2BMem(idxaddr,dm))
     let lobaseval = IntOf(Get2BMem(lobaseaddr,dm))
     let hilevelval = IntOf(Get2BMem(hileveladdr,dm))
     let tmp = match idxval, lobaseval with
               |INT a , INT b -> INT(a-b)
               |_ -> failwith "Wrong data"
     let um = Upd2BMem(idxaddr,dm,FromInt(tmp))
     let newflag = match tmp, hilevelval with
                   |INT a , INT b -> if (a >= 0s && a <= b)
                                     then flg
                                     else 
                                        let (ps, eo) = ex
                                        let eo1 = (cr3, OUT_OF_BOUNDS_EXC)
                                        let ex1 = (ps, eo1)
                                        let badIdxFlg = SetFlag(flg,OUT_OF_BOUNDS_FLG)
                                        let (cm,um,cs,ds,cr3,dr,ex,badIdxFlg) = RAISE(cm,dm,cs,ds,cr,dr,ex1,flg)                                        
                                        badIdxFlg
                   |_ -> failwith "Wrong data"
     (cm,um,cs,ds,cr3,dr,ex,newflag)

(*-------------------- Funkcje --------------------*)

let ADD(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a, count:int)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let mutable crx = cr + AddressSize
     let mutable list = []
     for i=1 to count do
         let opx = GetAddress(crx,cm)
         let opxaddr = dr + opx
         let opxvalue = typeOf(getBytes(opxaddr,dm))
         list <- opxvalue :: list
         crx <- crx + AddressSize
         printf $"Op%A{i}: %A{opxvalue} "
     
     let value = List.reduce (fun acc el ->
                                    match acc,el with
                                    |SINT a, SINT b -> SINT (a+b)
                                    |INT a, INT b -> INT (a+b)
                                    |DINT a, DINT b -> DINT (a+b)
                                    |LINT a, LINT b -> LINT (a+b)
                                    |BYTE a, BYTE b -> BYTE (a+b)
                                    |WORD a, WORD b -> WORD (a+b)
                                    |DWORD a, DWORD b -> DWORD (a+b)
                                    |LWORD a, LWORD b -> LWORD (a+b)
                                    |REAL a, REAL b -> REAL (a+b)
                                    |LREAL a, LREAL b -> LREAL (a+b)
                                    |_ -> failwith "Bad data"
                                    ) list

     printfn $"\nADD result: %A{value}"
     (cm, updBytes(raddr, dm, fromType(value)),cs,ds,crx,dr,ex,flg)

let SUB(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op1 = GetAddress(cr1,cm)
     let op1addr = dr + op1
     let cr2 = cr1 + AddressSize
     let op2 = GetAddress(cr2,cm)
     let op2addr = dr + op2
     let cr3 = cr2 + AddressSize
     let op1val = typeOf(getBytes(op1addr,dm))
     let op2val = typeOf(getBytes(op2addr,dm))

     let value = match op1val, op2val with
                 |SINT a, SINT b -> SINT (a-b)
                 |INT a, INT b -> INT (a-b)
                 |DINT a, DINT b -> DINT (a-b)
                 |LINT a, LINT b -> LINT (a-b) 
                 |BYTE a, BYTE b -> BYTE (a-b)
                 |WORD a, WORD b -> WORD (a-b)
                 |DWORD a, DWORD b -> DWORD (a-b)
                 |LWORD a, LWORD b -> LWORD (a-b)
                 |REAL a, REAL b -> REAL (a-b)
                 |LREAL a, LREAL b -> LREAL (a-b)
                 |TIME a, TIME b -> TIME (a-b)
                 |_ -> failwith "Błędne dane"

     printfn "Operand1: %A Operand2: %A \nWynik działania instrukcji SUB: %A" op1val op2val value
     (cm, updBytes(raddr, dm, fromType(value)), cs, ds, cr3, dr, ex, flg)

let MUL(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a, count:int)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let mutable crx = cr + AddressSize
     let mutable lista = []
     for i=1 to count do
         let opx = GetAddress(crx,cm)
         let opxaddr = dr + opx
         let opxvalue = typeOf(getBytes(opxaddr,dm))
         lista <- opxvalue :: lista
         crx <- crx + AddressSize
         printf "Op%A: %A " i opxvalue
     
     let value = List.reduce (fun acc el ->
                                    match acc,el with
                                    |SINT a, SINT b -> SINT (a*b)
                                    |INT a, INT b -> INT (a*b)
                                    |DINT a, DINT b -> DINT (a*b)
                                    |LINT a, LINT b -> LINT (a*b)
                                    |BYTE a, BYTE b -> BYTE (a*b)
                                    |WORD a, WORD b -> WORD (a*b)
                                    |DWORD a, DWORD b -> DWORD (a*b)
                                    |LWORD a, LWORD b -> LWORD (a*b)
                                    |REAL a, REAL b -> REAL (a*b)
                                    |LREAL a, LREAL b -> LREAL (a*b)
                                    |_ -> failwith "Błędne dane"
                                    ) lista

     printfn "\nWynik działania instrukcji MUL: %A" value
     (cm, updBytes(raddr, dm, fromType(value)),cs,ds,crx,dr,ex,flg)

let DIV (state:State, getBytes:(Address*Storage)->'a, updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a) : State =
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op1 = GetAddress(cr1,cm)
     let op1addr = dr + op1
     let cr2 = cr1 + AddressSize
     let op2 = GetAddress(cr2,cm)
     let op2addr = dr + op2
     let cr3 = cr2 + AddressSize
     let dividend = typeOf(getBytes(op1addr,dm))
     let divisor = typeOf(getBytes(op2addr,dm))
     match divisor with
     |SINT 0y | INT 0s | DINT 0 |LINT 0L | BYTE 0uy |WORD 0us |DWORD 0u | LWORD 0UL | REAL 0.0f | LREAL 0.0 -> 
        let (ps, eo) = ex
        let eo1 = (cr3, DIV_BY_ZERO_EXC)
        let ex1 = (ps, eo1) in RAISE(cm,dm,cs,ds,cr,dr,ex1,flg)
     |SINT _ | INT _ | DINT _ |LINT _ |BYTE _ |WORD _ 
     |DWORD _ | LWORD _  |REAL _ |LREAL _ -> 
        let value = match dividend, divisor with
                    |SINT a, SINT b -> SINT (a/b)
                    |INT a, INT b -> INT (a/b)
                    |DINT a, DINT b -> DINT (a/b)
                    |LINT a, LINT b -> LINT (a/b) 
                    |BYTE a, BYTE b -> BYTE (a/b)
                    |WORD a, WORD b -> WORD (a/b)
                    |DWORD a, DWORD b -> DWORD (a/b)
                    |LWORD a, LWORD b -> LWORD (a/b)
                    |REAL a, REAL b -> REAL (a/b)
                    |LREAL a, LREAL b -> LREAL (a/b)
                    |_ -> failwith "Wrong data"
//                 printfn "Operand1: %A Operand2: %A \nDIV result: %A" dividend divisor value
        (cm, updBytes(raddr, dm, fromType(value)), cs, ds, cr3, dr, ex, flg)
     |_ -> failwith "Wrong data";;



let MOD(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op1 = GetAddress(cr1,cm)
     let op1addr = dr + op1
     let cr2 = cr1 + AddressSize
     let op2 = GetAddress(cr2,cm)
     let op2addr = dr + op2
     let cr3 = cr2 + AddressSize
     let op1val = typeOf(getBytes(op1addr,dm))
     let op2val = typeOf(getBytes(op2addr,dm))

     let value = match op1val, op2val with
                 |SINT a, SINT b -> SINT (a%b)
                 |INT a, INT b -> INT (a%b)
                 |DINT a, DINT b -> DINT (a%b)
                 |LINT a, LINT b -> LINT (a%b) 
                 |BYTE a, BYTE b -> BYTE (a%b)
                 |WORD a, WORD b -> WORD (a%b)
                 |DWORD a, DWORD b -> DWORD (a%b)
                 |LWORD a, LWORD b -> LWORD (a%b)
                 |REAL a, REAL b -> REAL (a%b)
                 |LREAL a, LREAL b -> LREAL (a%b)
                 |_ -> failwith "Błędne dane"

     printfn "Operand1: %A Operand2: %A \nWynik działania instrukcji MOD: %A" op1val op2val value
     (cm, updBytes(raddr, dm, fromType(value)), cs, ds, cr3, dr, ex, flg)

let NEG(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op = GetAddress(cr1,cm)
     let opaddr = dr + op
     let cr2 = cr1 + AddressSize
     let opval = typeOf(getBytes(opaddr,dm))

     let value = match opval with
                 |SINT a -> SINT (-a)
                 |INT a -> INT (-a)
                 |DINT a -> DINT (-a)
                 |LINT a -> LINT (-a) 
                 |REAL a -> REAL (-a)
                 |LREAL a -> LREAL (-a)
                 |_ -> failwith "Błędne dane"

     printfn "Operand: %A \nWynik działania instrukcji NEG: %A" opval value
     (cm, updBytes(raddr, dm, fromType(value)), cs, ds, cr2, dr, ex, flg)

let NOT(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op = GetAddress(cr1,cm)
     let opaddr = dr + op
     let cr2 = cr1 + AddressSize
     let opval = typeOf(getBytes(opaddr,dm))

     let value = match opval with
                 |BOOL a -> BOOL( match a with
                                  |true -> false
                                  |false -> true ) 
                 |BYTE a -> BYTE (~~~a)
                 |WORD a -> WORD (~~~a)
                 |DWORD a -> DWORD (~~~a)
                 |LWORD a -> LWORD (~~~a)
                 |_ -> failwith "Błędne dane"

     printfn "Operand: %A \nWynik działania instrukcji NOT: %A" opval value
     (cm, updBytes(raddr, dm, fromType(value)), cs, ds, cr2, dr, ex, flg)

let ABS(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op = GetAddress(cr1,cm)
     let opaddr = dr + op
     let cr2 = cr1 + AddressSize
     let opval = typeOf(getBytes(opaddr,dm))

     let value = match opval with
                 |SINT a -> SINT (Math.Abs(a))
                 |INT a -> INT (Math.Abs(a))
                 |DINT a -> DINT (Math.Abs(a))
                 |LINT a -> LINT (Math.Abs(a)) 
                 |BYTE a -> BYTE (a)
                 |WORD a -> WORD (a)
                 |DWORD a -> DWORD (a)
                 |LWORD a -> LWORD (a)
                 |REAL a -> REAL (Math.Abs(a))
                 |LREAL a -> LREAL (Math.Abs(a))
                 |_ -> failwith "Błędne dane"

     printfn "Operand: %A \nWynik działania instrukcji ABS: %A" opval value
     (cm, updBytes(raddr, dm, fromType(value)), cs, ds, cr2, dr, ex, flg)

let SQRT(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op = GetAddress(cr1,cm)
     let opaddr = dr + op
     let cr2 = cr1 + AddressSize
     let opval = typeOf(getBytes(opaddr,dm))

     let value = match opval with
                 |REAL a ->  let x = Math.Sqrt( float <| a)
                             REAL ( float32 <| x )
                 |LREAL a -> LREAL ( Math.Sqrt(a) )
                 |_ -> failwith "Błędne dane"

     printfn "Operand: %A \nWynik działania instrukcji SQRT: %A" opval value
     (cm, updBytes(raddr, dm, fromType(value)), cs, ds, cr2, dr, ex, flg)

let LN(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op = GetAddress(cr1,cm)
     let opaddr = dr + op
     let cr2 = cr1 + AddressSize
     let opval = typeOf(getBytes(opaddr,dm))

     let value = match opval with
                 |REAL a ->  let x = Math.Log( float <| a)
                             REAL ( float32 <| x )
                 |LREAL a -> LREAL ( Math.Log(a) )
                 |_ -> failwith "Błędne dane"

     printfn "Operand: %A \nWynik działania instrukcji LN: %A" opval value
     (cm, updBytes(raddr, dm, fromType(value)), cs, ds, cr2, dr, ex, flg)

let LOG(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op = GetAddress(cr1,cm)
     let opaddr = dr + op
     let cr2 = cr1 + AddressSize
     let opval = typeOf(getBytes(opaddr,dm))

     let value = match opval with
                 |REAL a ->  let x = Math.Log10( float <| a)
                             REAL ( float32 <| x )
                 |LREAL a -> LREAL ( Math.Log10(a) )
                 |_ -> failwith "Błędne dane"

     printfn "Operand: %A \nWynik działania instrukcji LOG: %A" opval value
     (cm, updBytes(raddr, dm, fromType(value)), cs, ds, cr2, dr, ex, flg)

let EXP(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op = GetAddress(cr1,cm)
     let opaddr = dr + op
     let cr2 = cr1 + AddressSize
     let opval = typeOf(getBytes(opaddr,dm))

     let value = match opval with
                 |REAL a ->  let x = Math.Exp( float <| a)
                             REAL ( float32 <| x )
                 |LREAL a -> LREAL ( Math.Exp(a) )
                 |_ -> failwith "Błędne dane"

     printfn "Operand: %A \nWynik działania instrukcji EXP: %A" opval value
     (cm, updBytes(raddr, dm, fromType(value)), cs, ds, cr2, dr, ex, flg)

let SIN(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op = GetAddress(cr1,cm)
     let opaddr = dr + op
     let cr2 = cr1 + AddressSize
     let opval = typeOf(getBytes(opaddr,dm))

     let value = match opval with
                 |REAL a ->  let x = Math.Sin( float <| a)
                             REAL ( float32 <| x )
                 |LREAL a -> LREAL ( Math.Sin(a) )
                 |_ -> failwith "Błędne dane"

     printfn "Operand: %A \nWynik działania instrukcji SIN: %A" opval value
     (cm, updBytes(raddr, dm, fromType(value)), cs, ds, cr2, dr, ex, flg)

let COS(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op = GetAddress(cr1,cm)
     let opaddr = dr + op
     let cr2 = cr1 + AddressSize
     let opval = typeOf(getBytes(opaddr,dm))

     let value = match opval with
                 |REAL a ->  let x = Math.Cos( float <| a)
                             REAL ( float32 <| x )
                 |LREAL a -> LREAL ( Math.Cos(a) )
                 |_ -> failwith "Błędne dane"

     printfn "Operand: %A \nWynik działania instrukcji COS: %A" opval value
     (cm, updBytes(raddr, dm, fromType(value)), cs, ds, cr2, dr, ex, flg)

let TAN(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op = GetAddress(cr1,cm)
     let opaddr = dr + op
     let cr2 = cr1 + AddressSize
     let opval = typeOf(getBytes(opaddr,dm))

     let value = match opval with
                 |REAL a ->  let x = Math.Tan( float <| a)
                             REAL ( float32 <| x )
                 |LREAL a -> LREAL ( Math.Tan(a) )
                 |_ -> failwith "Błędne dane"

     printfn "Operand: %A \nWynik działania instrukcji TAN: %A" opval value
     (cm, updBytes(raddr, dm, fromType(value)), cs, ds, cr2, dr, ex, flg)

let ASIN(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op = GetAddress(cr1,cm)
     let opaddr = dr + op
     let cr2 = cr1 + AddressSize
     let opval = typeOf(getBytes(opaddr,dm))

     let value = match opval with
                 |REAL a ->  let x = Math.Asin( float <| a)
                             REAL ( float32 <| x )
                 |LREAL a -> LREAL ( Math.Asin(a) )
                 |_ -> failwith "Błędne dane"

     printfn "Operand: %A \nWynik działania instrukcji ASIN: %A" opval value
     (cm, updBytes(raddr, dm, fromType(value)), cs, ds, cr2, dr, ex, flg)

let ACOS(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op = GetAddress(cr1,cm)
     let opaddr = dr + op
     let cr2 = cr1 + AddressSize
     let opval = typeOf(getBytes(opaddr,dm))

     let value = match opval with
                 |REAL a ->  let x = Math.Acos( float <| a)
                             REAL ( float32 <| x )
                 |LREAL a -> LREAL ( Math.Acos(a) )
                 |_ -> failwith "Błędne dane"

     printfn "Operand: %A \nWynik działania instrukcji ACOS: %A" opval value
     (cm, updBytes(raddr, dm, fromType(value)), cs, ds, cr2, dr, ex, flg)

let ATAN(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op = GetAddress(cr1,cm)
     let opaddr = dr + op
     let cr2 = cr1 + AddressSize
     let opval = typeOf(getBytes(opaddr,dm))

     let value = match opval with
                 |REAL a ->  let x = Math.Atan( float <| a)
                             REAL ( float32 <| x )
                 |LREAL a -> LREAL ( Math.Atan(a) )
                 |_ -> failwith "Błędne dane"

     printfn "Operand: %A \nWynik działania instrukcji ATAN: %A" opval value
     (cm, updBytes(raddr, dm, fromType(value)), cs, ds, cr2, dr, ex, flg)

let TRUNC(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op = GetAddress(cr1,cm)
     let opaddr = dr + op
     let cr2 = cr1 + AddressSize
     let opval = typeOf(getBytes(opaddr,dm))

     let value = match opval with
                 |REAL a ->  DINT ( int32 <| a)
                 |LREAL a -> LINT ( int64 <| a)
                 |_ -> failwith "Błędne dane"

     printfn "Operand: %A \nWynik działania instrukcji TRUNC: %A" opval value
     (cm, updBytes(raddr, dm, fromType(value)), cs, ds, cr2, dr, ex, flg)

let ROUND(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op = GetAddress(cr1,cm)
     let opaddr = dr + op
     let cr2 = cr1 + AddressSize
     let opval = typeOf(getBytes(opaddr,dm))

     let value = match opval with
                 |REAL a ->  if a > 0.0f then DINT ( int32 <| (a + 0.5f) )
                                         else DINT ( int32 <| (a - 0.5f) )
                 |LREAL a -> if a > 0.0  then LINT ( int64 <| (a + 0.5) )
                                         else LINT ( int64 <| (a - 0.5) )
                 |_ -> failwith "Błędne dane"

     printfn "Operand: %A \nWynik działania instrukcji ROUND: %A" opval value
     (cm, updBytes(raddr, dm, fromType(value)), cs, ds, cr2, dr, ex, flg)

let AND(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a, count:int)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let mutable crx = cr + AddressSize
     let mutable lista = []
     for i=1 to count do
         let opx = GetAddress(crx,cm)
         let opxaddr = dr + opx
         let opxvalue = typeOf(getBytes(opxaddr,dm))
         lista <- opxvalue :: lista
         crx <- crx + AddressSize
         printf "Op%A: %A " i opxvalue
     
     let value = List.reduce (fun acc el ->
                                    match acc,el with
                                    |BOOL a, BOOL b -> BOOL( match a,b with 
                                                             |true, true -> true
                                                             |_ -> false ) 
                                    |BYTE a, BYTE b -> BYTE (a&&&b)
                                    |WORD a, WORD b -> WORD (a&&&b)
                                    |DWORD a, DWORD b -> DWORD (a&&&b)
                                    |LWORD a, LWORD b -> LWORD (a&&&b)
                                    |_ -> failwith "Błędne dane"
                                    ) lista

     printfn "\nWynik działania instrukcji AND: %A" value
     (cm, updBytes(raddr, dm, fromType(value)),cs,ds,crx,dr,ex,flg)

let OR(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a, count:int)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let mutable crx = cr + AddressSize
     let mutable lista = []
     for i=1 to count do
         let opx = GetAddress(crx,cm)
         let opxaddr = dr + opx
         let opxvalue = typeOf(getBytes(opxaddr,dm))
         lista <- opxvalue :: lista
         crx <- crx + AddressSize
         printf "Op%A: %A " i opxvalue
     
     let value = List.reduce (fun acc el ->
                                    match acc,el with
                                    |BOOL a, BOOL b -> BOOL( match a,b with 
                                                             |false, false -> false
                                                             |_ -> true ) 
                                    |BYTE a, BYTE b -> BYTE (a|||b)
                                    |WORD a, WORD b -> WORD (a|||b)
                                    |DWORD a, DWORD b -> DWORD (a|||b)
                                    |LWORD a, LWORD b -> LWORD (a|||b)
                                    |_ -> failwith "Błędne dane"
                                    ) lista

     printfn "\nWynik działania instrukcji OR: %A" value
     (cm, updBytes(raddr, dm, fromType(value)),cs,ds,crx,dr,ex,flg)

let XOR(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a, count:int)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let mutable crx = cr + AddressSize
     let mutable lista = []
     for i=1 to count do
         let opx = GetAddress(crx,cm)
         let opxaddr = dr + opx
         let opxvalue = typeOf(getBytes(opxaddr,dm))
         lista <- opxvalue :: lista
         crx <- crx + AddressSize
         printf "Op%A: %A " i opxvalue
     
     let value = List.reduce (fun acc el ->
                                    match acc,el with
                                    |BOOL a, BOOL b -> BOOL( match a,b with 
                                                             |true, false -> true
                                                             |false, true -> true
                                                             |_ -> false ) 
                                    |BYTE a, BYTE b -> BYTE (a^^^b)
                                    |WORD a, WORD b -> WORD (a^^^b)
                                    |DWORD a, DWORD b -> DWORD (a^^^b)
                                    |LWORD a, LWORD b -> LWORD (a^^^b)
                                    |_ -> failwith "Błędne dane"
                                    ) lista

     printfn "\nWynik działania instrukcji XOR: %A" value
     (cm, updBytes(raddr, dm, fromType(value)),cs,ds,crx,dr,ex,flg)

let SHL(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op1 = GetAddress(cr1,cm)
     let op1addr = dr + op1
     let cr2 = cr1 + AddressSize
     let op2 = GetAddress(cr2,cm)
     let op2addr = dr + op2
     let cr3 = cr2 + AddressSize
    
     let op1val = typeOf(getBytes(op1addr,dm))
     let op2val = IntOf(Get2BMem(op2addr,dm))

     let value = match op1val, op2val with
                 |BYTE a , INT b  -> BYTE  ( a <<< (int32 <| b))             
                 |WORD a , INT b  -> WORD  ( a <<< (int32 <| b))
                 |DWORD a , INT b -> DWORD ( a <<< (int32 <| b))  
                 |LWORD a , INT b -> LWORD ( a <<< (int32 <| b))  
                 | _ -> failwith "Błędne dane"

     printfn "Operand: %A Przesunięcie: %A \nWynik działania instrukcji SHL: %A" op1val op2val value
     (cm, updBytes(raddr, dm, fromType(value)),cs,ds,cr3,dr,ex,flg)

let SHR(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op1 = GetAddress(cr1,cm)
     let op1addr = dr + op1
     let cr2 = cr1 + AddressSize
     let op2 = GetAddress(cr2,cm)
     let op2addr = dr + op2
     let cr3 = cr2 + AddressSize
     
     let op1val = typeOf(getBytes(op1addr,dm))
     let op2val = IntOf(Get2BMem(op2addr,dm))

     let value = match op1val , op2val with
                 |BYTE a , INT b  -> BYTE  ( a >>> (int32 <| b))             
                 |WORD a , INT b  -> WORD  ( a >>> (int32 <| b))
                 |DWORD a , INT b -> DWORD ( a >>> (int32 <| b))  
                 |LWORD a , INT b -> LWORD ( a >>> (int32 <| b))  
                 | _ -> failwith "Błędne dane"

     printfn "Operand: %A Przesunięcie: %A \nWynik działania instrukcji SHR: %A" op1val op2val value
     (cm, updBytes(raddr, dm, fromType(value)),cs,ds,cr3,dr,ex,flg)

let ROL(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op = GetAddress(cr1,cm)
     let opaddr = dr + op
     let cr2 = cr1 + AddressSize
     let num = GetAddress(cr2,cm)
     let numaddr = dr + num
     let cr3 = cr2 + AddressSize
     let opval = typeOf(getBytes(opaddr,dm))
     let numval = IntOf(Get2BMem(numaddr,dm))

     let value = match opval, numval with
                 |BYTE a  , INT b -> BYTE( (a<<<(int32 <| b)) |||  (a>>>(8-(int32 <| b)))  )
                 |WORD a  , INT b -> WORD( (a<<<(int32 <| b)) |||  (a>>>(16-(int32 <| b))) )
                 |DWORD a , INT b -> DWORD( (a<<<(int32 <| b)) ||| (a>>>(32-(int32 <| b))) )
                 |LWORD a , INT b -> LWORD( (a<<<(int32 <| b)) ||| (a>>>(64-(int32 <| b))) )
                 |_ -> failwith "Błędne dane"

     printfn "Operand: %A Num: %A \nWynik działania instrukcji ROL: %A" opval numval value
     (cm, updBytes(raddr,dm,fromType(value)),cs,ds,cr3,dr,ex,flg)

let ROR(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op = GetAddress(cr1,cm)
     let opaddr = dr + op
     let cr2 = cr1 + AddressSize
     let num = GetAddress(cr2,cm)
     let numaddr = dr + num
     let cr3 = cr2 + AddressSize
     let opval = typeOf(getBytes(opaddr,dm))
     let numval = IntOf(Get2BMem(numaddr,dm))

     let value = match opval, numval with
                 |BYTE a  , INT b -> BYTE(  (a>>>(int32 <| b)) |||  (a<<<(8-(int32 <| b))) )
                 |WORD a  , INT b -> WORD( (a>>>(int32 <| b)) |||  (a<<<(16-(int32 <| b))) )
                 |DWORD a , INT b -> DWORD( (a>>>(int32 <| b)) ||| (a<<<(32-(int32 <| b))) )
                 |LWORD a , INT b -> LWORD( (a>>>(int32 <| b)) ||| (a<<<(64-(int32 <| b))) )
                 |_ -> failwith "Błędne dane"

     printfn "Operand: %A Num: %A \nWynik działania instrukcji ROR: %A" opval numval value
     (cm, updBytes(raddr,dm,fromType(value)),cs,ds,cr3,dr,ex,flg)


let SEL(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let sel = GetAddress(cr1,cm)
     let seladdr = dr + sel
     let cr2 = cr1 + AddressSize
     let op1 = GetAddress(cr2,cm)
     let op1addr = dr + op1
     let cr3 = cr2 + AddressSize
     let op2 = GetAddress(cr3,cm)
     let op2addr = dr + op2
     let cr4 = cr3 + AddressSize
     
     let bv = BoolOf(Get1BMem(seladdr,dm))
     let op1val = typeOf(getBytes(op1addr,dm))
     let op2val = typeOf(getBytes(op2addr,dm))
     let result = match bv with
                  |BOOL false -> op1val
                  |BOOL true  -> op2val 
                  |_ -> failwith "Błędne dane"

     printfn "Selektor: %A Operand1: %A Operand2: %A \nWynik działania instrukcji SEL: %A" bv op1val op2val result
     (cm,updBytes(raddr,dm,fromType(result)),cs,ds,cr4,dr,ex,flg)

let LIMIT(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let war = GetAddress(cr1,cm)
     let waraddr = dr + war
     let cr2 = cr1 + AddressSize
     let min = GetAddress(cr2,cm)
     let minaddr = dr + min
     let cr3 = cr2 + AddressSize
     let max = GetAddress(cr3,cm)
     let maxaddr = dr + max
     let cr4 = cr3 + AddressSize

     let value = typeOf(getBytes(waraddr,dm))
     let minval = typeOf(getBytes(minaddr,dm))
     let maxval = typeOf(getBytes(maxaddr,dm))

     let result = if value < minval then minval
                  elif value > maxval then maxval
                  else value

     printfn "Value: %A MinValue: %A MaxValue: %A \nWynik działania instrukcji LIMIT: %A" value minval maxval result
     (cm,updBytes(raddr,dm, fromType(result)),cs,ds,cr4,dr,ex,flg)

let MAX(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op1 = GetAddress(cr1,cm)
     let op1addr = dr + op1
     let cr2 = cr1 + AddressSize
     let op2 = GetAddress(cr2,cm)
     let op2addr = dr + op2
     let cr3 = cr2 + AddressSize

     let op1val = typeOf(getBytes(op1addr,dm))
     let op2val = typeOf(getBytes(op2addr,dm))

     let result =  if op1val < op2val then op2val
                   else op1val
     
     printfn "Operand1: %A Operand2: %A \nWynik działania instrukcji MAX: %A" op1val op2val result
     (cm,updBytes(raddr,dm,fromType(result)),cs,ds,cr3,dr,ex,flg)

let MIN(state:State, getBytes:(Address*Storage)->'a , updBytes:(Address*Storage*'a)->Storage, typeOf: 'a -> ValueType, fromType: ValueType -> 'a)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op1 = GetAddress(cr1,cm)
     let op1addr = dr + op1
     let cr2 = cr1 + AddressSize
     let op2 = GetAddress(cr2,cm)
     let op2addr = dr + op2
     let cr3 = cr2 + AddressSize

     let op1val = typeOf(getBytes(op1addr,dm))
     let op2val = typeOf(getBytes(op2addr,dm))

     let result = if op1val < op2val then op1val
                  else op2val
     
     printfn "Operand1: %A Operand2: %A \nWynik działania instrukcji MIN: %A" op1val op2val result
     (cm,updBytes(raddr,dm,fromType(result)),cs,ds,cr3,dr,ex,flg)

let GT(state:State, getBytes:(Address*Storage)->'a , typeOf: 'a -> ValueType)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op1 = GetAddress(cr1,cm)
     let op1addr = dr + op1
     let cr2 = cr1 + AddressSize
     let op2 = GetAddress(cr2,cm)
     let op2addr = dr + op2
     let cr3 = cr2 + AddressSize

     let op1val = typeOf(getBytes(op1addr,dm))
     let op2val = typeOf(getBytes(op2addr,dm))
     let cmp = BOOL ( op1val > op2val)

     printfn "Operand1: %A Operand2: %A \nWynik działania instrukcji GT: %A" op1val op2val cmp
     (cm,Upd1BMem(raddr, dm, FromBool(cmp)),cs,ds,cr3,dr,ex,flg)

let GE(state:State, getBytes:(Address*Storage)->'a , typeOf: 'a -> ValueType)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op1 = GetAddress(cr1,cm)
     let op1addr = dr + op1
     let cr2 = cr1 + AddressSize
     let op2 = GetAddress(cr2,cm)
     let op2addr = dr + op2
     let cr3 = cr2 + AddressSize

     let op1val = typeOf(getBytes(op1addr,dm))
     let op2val = typeOf(getBytes(op2addr,dm))
     let cmp = BOOL ( op1val >= op2val)

     printfn "Operand1: %A Operand2: %A \nWynik działania instrukcji GE: %A" op1val op2val cmp
     (cm,Upd1BMem(raddr, dm, FromBool(cmp)),cs,ds,cr3,dr,ex,flg)

let LT(state:State, getBytes:(Address*Storage)->'a , typeOf: 'a -> ValueType)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op1 = GetAddress(cr1,cm)
     let op1addr = dr + op1
     let cr2 = cr1 + AddressSize
     let op2 = GetAddress(cr2,cm)
     let op2addr = dr + op2
     let cr3 = cr2 + AddressSize

     let op1val = typeOf(getBytes(op1addr,dm))
     let op2val = typeOf(getBytes(op2addr,dm))
     let cmp = BOOL ( op1val < op2val)
     
     printfn "Operand1: %A Operand2: %A \nWynik działania instrukcji LT: %A" op1val op2val cmp
     (cm,Upd1BMem(raddr, dm, FromBool(cmp)),cs,ds,cr3,dr,ex,flg)

let LE(state:State, getBytes:(Address*Storage)->'a , typeOf: 'a -> ValueType)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op1 = GetAddress(cr1,cm)
     let op1addr = dr + op1
     let cr2 = cr1 + AddressSize
     let op2 = GetAddress(cr2,cm)
     let op2addr = dr + op2
     let cr3 = cr2 + AddressSize

     let op1val = typeOf(getBytes(op1addr,dm))
     let op2val = typeOf(getBytes(op2addr,dm))
     let cmp = BOOL ( op1val <= op2val)
     
     printfn "Operand1: %A Operand2: %A \nWynik działania instrukcji LE: %A" op1val op2val cmp
     (cm,Upd1BMem(raddr, dm, FromBool(cmp)),cs,ds,cr3,dr,ex,flg)

let EQ(state:State, getBytes:(Address*Storage)->'a , typeOf: 'a -> ValueType)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op1 = GetAddress(cr1,cm)
     let op1addr = dr + op1
     let cr2 = cr1 + AddressSize
     let op2 = GetAddress(cr2,cm)
     let op2addr = dr + op2
     let cr3 = cr2 + AddressSize

     let op1val = typeOf(getBytes(op1addr,dm))
     let op2val = typeOf(getBytes(op2addr,dm))
     let cmp = BOOL ( op1val = op2val)
     
     printfn "Operand1: %A Operand2: %A \nWynik działania instrukcji EQ: %A" op1val op2val cmp
     (cm,Upd1BMem(raddr, dm, FromBool(cmp)),cs,ds,cr3,dr,ex,flg)

let NE(state:State, getBytes:(Address*Storage)->'a , typeOf: 'a -> ValueType)=
     let (cm,dm,cs,ds,cr,dr,ex,flg) = state
     let r = GetAddress(cr,cm)
     let raddr = dr + r
     let cr1 = cr + AddressSize
     let op1 = GetAddress(cr1,cm)
     let op1addr = dr + op1
     let cr2 = cr1 + AddressSize
     let op2 = GetAddress(cr2,cm)
     let op2addr = dr + op2
     let cr3 = cr2 + AddressSize

     let op1val = typeOf(getBytes(op1addr,dm))
     let op2val = typeOf(getBytes(op2addr,dm))
     let cmp = BOOL ( op1val <> op2val)

     printfn "Operand1: %A Operand2: %A \nWynik działania instrukcji NE: %A" op1val op2val cmp
     (cm,Upd1BMem(raddr, dm, FromBool(cmp)),cs,ds,cr3,dr,ex,flg)


let CUR_TIME(state:State):State = 
    let (cm,dm,cs,ds,cr,dr,ex,flg) = state
    let r = GetAddress(cr,cm)
    let raddr = dr + r
    let cr1 = cr + AddressSize
    
    let value = DWORD(uint32(DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond))
    //printfn "Operand1: %X \nWynik działania CUR_TIME: %A" raddr value
    (cm, Upd4BMem(raddr,dm,FromDWord(value)), cs, ds, cr1, dr, ex, flg)

let FINIT(state:State):State = 
    state

(*-------------------- Konwersje typów --------------------*)

let INT_TO_REAL(state:State):State = 
    let (cm,dm,cs,ds,cr,dr,ex,flg) = state
    let r = GetAddress(cr,cm)
    let raddr = dr + r
    let cr1 = cr + AddressSize
    let op = GetAddress(cr1,cm)
    let opaddr = dr + op
    let cr2 = cr1 + AddressSize
    
    let opval = IntOf(Get2BMem(opaddr,dm))

    let value = match opval with
                |INT a -> REAL(float32 <| a)
                |_     -> failwith "Błędne dane"

    printfn "Operand: %A \nWynik działania instrukcji INT_TO_REAL: %A" opval value
    (cm, Upd4BMem(raddr,dm,FromReal(value)), cs, ds, cr2, dr, ex, flg)

let DINT_TO_REAL(state:State):State = 
    let (cm,dm,cs,ds,cr,dr,ex,flg) = state
    let r = GetAddress(cr,cm)
    let raddr = dr + r
    let cr1 = cr + AddressSize
    let op = GetAddress(cr1,cm)
    let opaddr = dr + op
    let cr2 = cr1 + AddressSize
    
    let opval = DIntOf(Get4BMem(opaddr,dm))

    let value = match opval with
                |DINT a -> REAL(float32 <| a)
                |_     -> failwith "Błędne dane"


    printfn "Operand: %A \nWynik działania instrukcji DINT_TO_REAL: %A" opval value
    (cm, Upd4BMem(raddr,dm,FromReal(value)), cs, ds, cr2, dr, ex, flg)

let REAL_TO_INT(state:State):State = 
    let (cm,dm,cs,ds,cr,dr,ex,flg) = state
    let r = GetAddress(cr,cm)
    let raddr = dr + r
    let cr1 = cr + AddressSize
    let op = GetAddress(cr1,cm)
    let opaddr = dr + op
    let cr2 = cr1 + AddressSize
    
    let opval = RealOf(Get4BMem(opaddr,dm))

    let value = match opval with
                |REAL a -> INT(int16 <| a)
                |_     -> failwith "Błędne dane"
    
    printfn "Operand: %A \nWynik działania instrukcji REAL_TO_INT: %A" opval value
    (cm, Upd2BMem(raddr,dm,FromInt(value)), cs, ds, cr2, dr, ex, flg)

let INT_TO_BOOL(state:State):State = 
    let (cm,dm,cs,ds,cr,dr,ex,flg) = state
    let r = GetAddress(cr,cm)
    let raddr = dr + r
    let cr1 = cr + AddressSize
    let op = GetAddress(cr1,cm)
    let opaddr = dr + op
    let cr2 = cr1 + AddressSize
    
    let opval = IntOf(Get2BMem(opaddr,dm))

    let value = match opval with
                |INT a -> BOOL(a <> 0s)
                |_     -> failwith "Błędne dane"

    printfn "Operand: %A \nWynik działania instrukcji INT_TO_BOOL: %A" opval value
    (cm, Upd1BMem(raddr,dm,FromBool(value)), cs, ds, cr2, dr, ex, flg)

let INT_TO_DINT(state:State):State = 
    let (cm,dm,cs,ds,cr,dr,ex,flg) = state
    let r = GetAddress(cr,cm)
    let raddr = dr + r
    let cr1 = cr + AddressSize
    let op = GetAddress(cr1,cm)
    let opaddr = dr + op
    let cr2 = cr1 + AddressSize
    
    let opval = IntOf(Get2BMem(opaddr,dm))

    let value = match opval with
                |INT a -> DINT(int32 <| a)
                |_     -> failwith "Błędne dane"

    printfn "Operand: %A \nWynik działania instrukcji INT_TO_DINT: %A" opval value
    (cm, Upd4BMem(raddr,dm,FromDInt(value)), cs, ds, cr2, dr, ex, flg)

let REAL_TO_LREAL(state:State):State = 
    let (cm,dm,cs,ds,cr,dr,ex,flg) = state
    let r = GetAddress(cr,cm)
    let raddr = dr + r
    let cr1 = cr + AddressSize
    let op = GetAddress(cr1,cm)
    let opaddr = dr + op
    let cr2 = cr1 + AddressSize
    
    let opval = RealOf(Get4BMem(opaddr,dm))

    let value = match opval with
                |REAL a -> LREAL(float <| a)
                |_     -> failwith "Błędne dane"
    
    printfn "Operand: %A \nWynik działania instrukcji REAL_TO_LREAL: %A" opval value
    (cm, Upd8BMem(raddr,dm,FromLReal(value)), cs, ds, cr2, dr, ex, flg)

let LREAL_TO_REAL(state:State):State = 
    let (cm,dm,cs,ds,cr,dr,ex,flg) = state
    let r = GetAddress(cr,cm)
    let raddr = dr + r
    let cr1 = cr + AddressSize
    let op = GetAddress(cr1,cm)
    let opaddr = dr + op
    let cr2 = cr1 + AddressSize
    
    let opval = LRealOf(Get8BMem(opaddr,dm))

    let value = match opval with
                |LREAL a -> REAL(float32 <| a)
                |_     -> failwith "Błędne dane"

    printfn "Operand: %A \nWynik działania instrukcji LREAL_TO_REAL: %A" opval value
    (cm, Upd4BMem(raddr,dm,FromReal(value)), cs, ds, cr2, dr, ex, flg)

let DWORD_TO_REAL(state:State):State = 
    let (cm,dm,cs,ds,cr,dr,ex,flg) = state
    let r = GetAddress(cr,cm)
    let raddr = dr + r
    let cr1 = cr + AddressSize
    let op = GetAddress(cr1,cm)
    let opaddr = dr + op
    let cr2 = cr1 + AddressSize
    
    let opval = DWordOf(Get4BMem(opaddr,dm))

    let value = match opval with
                |DWORD a -> REAL(float32 <| a)
                |_     -> failwith "Błędne dane"

    printfn "Operand: %A \nWynik działania instrukcji DWORD_TO_REAL: %A" opval value
    (cm, Upd4BMem(raddr,dm,FromReal(value)), cs, ds, cr2, dr, ex, flg)

let REAL_TO_DWORD(state:State):State = 
    let (cm,dm,cs,ds,cr,dr,ex,flg) = state
    let r = GetAddress(cr,cm)
    let raddr = dr + r
    let cr1 = cr + AddressSize
    let op = GetAddress(cr1,cm)
    let opaddr = dr + op
    let cr2 = cr1 + AddressSize
    
    let opval = RealOf(Get4BMem(opaddr,dm))

    let value = match opval with
                |REAL a -> DWORD(uint32 <| a)
                |_     -> failwith "Błędne dane"

    printfn "Operand: %A \nWynik działania instrukcji REAL_TO_DWORD: %A" opval value
    (cm, Upd4BMem(raddr,dm,FromDWord(value)), cs, ds, cr2, dr, ex, flg)


let printByteArrayAsHex (byteArray: byte[]) =
    for byteValue in byteArray do
        printf "%02X " byteValue
    printfn ""



[<EntryPoint>]
let main argv =  
    
    //do działania w pętli

    let mutable flaga = true

    //stos kodu i stos danych
    let mutable codeStack : Stack = List.Empty
    let mutable dataStack : Stack = List.Empty

    //pamięć kodu i pamięć danych
    let mutable codeMemoryStorage : Storage = Array.init 65536 (fun _ -> 0uy)
    let mutable dataMemoryStorage : Storage = Array.init 65536 (fun _ -> 0uy)
    
    //rejestr instrukcji i rejestr danych
    let mutable codeReg : Address = LanguagePrimitives.GenericZero
    let mutable dataReg : Address = LanguagePrimitives.GenericZero

    //flagi
    let mutable flags : Flags = 0us
    
    
    let codeMemory : Memory = (fun (address,storage) -> storage.[int <| address])
    let dataMemory : Memory = (fun (address,storage) -> storage.[int <| address])

    let excObj : ExcObj = (LanguagePrimitives.GenericZero, 0);
    let mutable protStack : ProtStack = List.Empty
    let excState : ExcState = (protStack,excObj)

    //stan
    let mutable state : State = (codeMemoryStorage,dataMemoryStorage,codeStack,dataStack,codeReg,dataReg,excState,flags)
   
(*------------------------- Test działania funkcji -------------------------*)

    if argv.Length >= 1
    then
        file_path <- argv[0]
    else 
        printfn "Podaj ścieżkę do pliku zawierającego kod bajtowy programu do wykonania:\n "
        file_path <- System.Console.ReadLine()

    try

       System.IO.File.OpenRead(file_path).Read(codeMemoryStorage,0,65536) |> ignore 
    with
       | :? System.IO.IOException    -> printf "Błędna ścieżka!"
                                        flaga <- false 
       | :? System.ArgumentException -> printf "Nie podałeś ścieżki do pliku!"
                                        flaga <- false
    
    if(display_memory = true) then
        printfn "Pamięć kodu:"
        printfn "%A" <| Array.take 50 codeMemoryStorage
        printfn "Pamięć danych:"
        let monitoredData = Array.take 10 dataMemoryStorage
        printByteArrayAsHex monitoredData
    
    while flaga do 

        let mutable startCycle : DateTime = DateTime.Now;

        loop <- true
        printfn "\nWykonuję instrukcje CPDEV:\n"
        Thread.Sleep(1000)
        while (loop) do

            let ig = Get1BMem(codeReg,codeMemoryStorage)
            codeReg <- codeReg .+. 1
            let ic = Get1BMem(codeReg,codeMemoryStorage)
            codeReg <- codeReg .+. 1

            // printfn "Instrukcja: %X %X" ig ic

            state <- (codeMemoryStorage,dataMemoryStorage,codeStack,dataStack,codeReg,dataReg,excState,flags)

            match ig with
                  |0x01uy -> match ic with
                                   |a when (a &&& 0x0Fuy) = 10uy -> state <- ADD(state, Get8BMem, Upd8BMem, LRealOf, FromLReal,(int <| (a &&& 0xF0uy))/16)
                                   |b when (b &&& 0x0Fuy) = 9uy  -> state <- ADD(state, Get4BMem, Upd4BMem, RealOf, FromReal,(int <| (b &&& 0xF0uy))/16)
                                   |c when (c &&& 0x0Fuy) = 8uy  -> state <- ADD(state, Get8BMem, Upd8BMem, LWordOf, FromLWord,(int <| (c &&& 0xF0uy))/16)
                                   |d when (d &&& 0x0Fuy) = 7uy  -> state <- ADD(state, Get4BMem, Upd4BMem, DWordOf, FromDWord,(int <| (d &&& 0xF0uy))/16)
                                   |e when (e &&& 0x0Fuy) = 6uy  -> state <- ADD(state, Get2BMem, Upd2BMem, WordOf, FromWord,(int <| (e &&& 0xF0uy))/16)
                                   |f when (f &&& 0x0Fuy) = 5uy  -> state <- ADD(state, Get1BMem, Upd1BMem, ByteOf, FromByte,(int <| (f &&& 0xF0uy))/16)
                                   |g when (g &&& 0x0Fuy) = 4uy  -> state <- ADD(state, Get8BMem, Upd8BMem, LIntOf, FromLInt,(int <| (g &&& 0xF0uy))/16)
                                   |h when (h &&& 0x0Fuy) = 3uy  -> state <- ADD(state, Get4BMem, Upd4BMem, DIntOf, FromDInt,(int <| (h &&& 0xF0uy))/16)
                                   |i when (i &&& 0x0Fuy) = 2uy  -> state <- ADD(state, Get2BMem, Upd2BMem, IntOf, FromInt,(int <| (i &&& 0xF0uy))/16)
                                   |j when (j &&& 0x0Fuy) = 1uy  -> state <- ADD(state, Get1BMem, Upd1BMem, SIntOf, FromSInt,(int <| (j &&& 0xF0uy))/16)                           
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x02uy -> match ic with
                                   |0x01uy -> state <- SUB(state, Get1BMem, Upd1BMem, SIntOf, FromSInt)
                                   |0x02uy -> state <- SUB(state, Get2BMem, Upd2BMem, IntOf, FromInt)
                                   |0x03uy -> state <- SUB(state, Get4BMem, Upd4BMem, DIntOf, FromDInt)
                                   |0x04uy -> state <- SUB(state, Get8BMem, Upd8BMem, LIntOf, FromLInt)
                                   |0x05uy -> state <- SUB(state, Get1BMem, Upd1BMem, ByteOf, FromByte)
                                   |0x06uy -> state <- SUB(state, Get2BMem, Upd2BMem, WordOf, FromWord)
                                   |0x07uy -> state <- SUB(state, Get4BMem, Upd4BMem, DWordOf, FromDWord)
                                   |0x08uy -> state <- SUB(state, Get8BMem, Upd8BMem, LWordOf, FromLWord)
                                   |0x09uy -> state <- SUB(state, Get4BMem, Upd4BMem, RealOf, FromReal)
                                   |0x0Auy -> state <- SUB(state, Get8BMem, Upd8BMem, LRealOf, FromLReal)
                                   |0x0Buy -> state <- SUB(state, Get4BMem, Upd4BMem, TimeOf, FromTime)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x03uy -> match ic with
                                   |a when (a &&& 0x0Fuy) = 10uy -> state <- MUL(state, Get8BMem, Upd8BMem, LRealOf, FromLReal,(int <| (a &&& 0xF0uy))/16)
                                   |b when (b &&& 0x0Fuy) = 9uy  -> state <- MUL(state, Get4BMem, Upd4BMem, RealOf, FromReal,(int <| (b &&& 0xF0uy))/16)
                                   |c when (c &&& 0x0Fuy) = 8uy  -> state <- MUL(state, Get8BMem, Upd8BMem, LWordOf, FromLWord,(int <| (c &&& 0xF0uy))/16)
                                   |d when (d &&& 0x0Fuy) = 7uy  -> state <- MUL(state, Get4BMem, Upd4BMem, DWordOf, FromDWord,(int <| (d &&& 0xF0uy))/16)
                                   |e when (e &&& 0x0Fuy) = 6uy  -> state <- MUL(state, Get2BMem, Upd2BMem, WordOf, FromWord,(int <| (e &&& 0xF0uy))/16)
                                   |f when (f &&& 0x0Fuy) = 5uy  -> state <- MUL(state, Get1BMem, Upd1BMem, ByteOf, FromByte,(int <| (f &&& 0xF0uy))/16)
                                   |g when (g &&& 0x0Fuy) = 4uy  -> state <- MUL(state, Get8BMem, Upd8BMem, LIntOf, FromLInt,(int <| (g &&& 0xF0uy))/16)
                                   |h when (h &&& 0x0Fuy) = 3uy  -> state <- MUL(state, Get4BMem, Upd4BMem, DIntOf, FromDInt,(int <| (h &&& 0xF0uy))/16)
                                   |i when (i &&& 0x0Fuy) = 2uy  -> state <- MUL(state, Get2BMem, Upd2BMem, IntOf, FromInt,(int <| (i &&& 0xF0uy))/16)
                                   |j when (j &&& 0x0Fuy) = 1uy  -> state <- MUL(state, Get1BMem, Upd1BMem, SIntOf, FromSInt,(int <| (j &&& 0xF0uy))/16) 
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x04uy -> match ic with
                                   |0x01uy -> state <- DIV(state, Get1BMem, Upd1BMem, SIntOf, FromSInt)
                                   |0x02uy -> state <- DIV(state, Get2BMem, Upd2BMem, IntOf, FromInt)
                                   |0x03uy -> state <- DIV(state, Get4BMem, Upd4BMem, DIntOf, FromDInt)
                                   |0x04uy -> state <- DIV(state, Get8BMem, Upd8BMem, LIntOf, FromLInt)
                                   |0x05uy -> state <- DIV(state, Get1BMem, Upd1BMem, ByteOf, FromByte)
                                   |0x06uy -> state <- DIV(state, Get2BMem, Upd2BMem, WordOf, FromWord)
                                   |0x07uy -> state <- DIV(state, Get4BMem, Upd4BMem, DWordOf, FromDWord)
                                   |0x08uy -> state <- DIV(state, Get8BMem, Upd8BMem, LWordOf, FromLWord)
                                   |0x09uy -> state <- DIV(state, Get4BMem, Upd4BMem, RealOf, FromReal)
                                   |0x0Auy -> state <- DIV(state, Get8BMem, Upd8BMem, LRealOf, FromLReal)
                                   |0x11uy -> state <- MOD(state, Get1BMem, Upd1BMem, SIntOf, FromSInt)
                                   |0x12uy -> state <- MOD(state, Get2BMem, Upd2BMem, IntOf, FromInt)
                                   |0x13uy -> state <- MOD(state, Get4BMem, Upd4BMem, DIntOf, FromDInt)
                                   |0x14uy -> state <- MOD(state, Get8BMem, Upd8BMem, LIntOf, FromLInt)
                                   |0x15uy -> state <- MOD(state, Get1BMem, Upd1BMem, ByteOf, FromByte)
                                   |0x16uy -> state <- MOD(state, Get2BMem, Upd2BMem, WordOf, FromWord)
                                   |0x17uy -> state <- MOD(state, Get4BMem, Upd4BMem, DWordOf, FromDWord)
                                   |0x18uy -> state <- MOD(state, Get8BMem, Upd8BMem, LWordOf, FromLWord)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x05uy -> match ic with
                                   |0x01uy -> state <- NEG(state, Get1BMem, Upd1BMem, SIntOf, FromSInt)
                                   |0x02uy -> state <- NEG(state, Get2BMem, Upd2BMem, IntOf, FromInt)
                                   |0x03uy -> state <- NEG(state, Get4BMem, Upd4BMem, DIntOf, FromDInt)
                                   |0x04uy -> state <- NEG(state, Get8BMem, Upd8BMem, LIntOf, FromLInt)
                                   |0x07uy -> state <- NEG(state, Get4BMem, Upd4BMem, RealOf, FromReal)
                                   |0x09uy -> state <- NEG(state, Get8BMem, Upd8BMem, LRealOf, FromLReal)
                                   |0x10uy -> state <- NOT(state, Get1BMem, Upd1BMem, BoolOf, FromBool)
                                   |0x11uy -> state <- NOT(state, Get1BMem, Upd1BMem, ByteOf, FromByte)
                                   |0x12uy -> state <- NOT(state, Get2BMem, Upd2BMem, WordOf, FromWord)
                                   |0x13uy -> state <- NOT(state, Get4BMem, Upd4BMem, DWordOf, FromDWord)
                                   |0x14uy -> state <- NOT(state, Get8BMem, Upd8BMem, LWordOf, FromLWord)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x06uy -> match ic with
                                   |0x11uy -> state <- ABS(state, Get1BMem, Upd1BMem, SIntOf, FromSInt)
                                   |0x12uy -> state <- ABS(state, Get2BMem, Upd2BMem, IntOf, FromInt)
                                   |0x13uy -> state <- ABS(state, Get4BMem, Upd4BMem, DIntOf, FromDInt)
                                   |0x14uy -> state <- ABS(state, Get8BMem, Upd8BMem, LIntOf, FromLInt)
                                   |0x15uy -> state <- ABS(state, Get1BMem, Upd1BMem, ByteOf, FromByte)
                                   |0x16uy -> state <- ABS(state, Get2BMem, Upd2BMem, WordOf, FromWord)
                                   |0x17uy -> state <- ABS(state, Get4BMem, Upd4BMem, DWordOf, FromDWord)
                                   |0x18uy -> state <- ABS(state, Get8BMem, Upd8BMem, LWordOf, FromLWord)
                                   |0x19uy -> state <- ABS(state, Get4BMem, Upd4BMem, RealOf, FromReal)
                                   |0x1Auy -> state <- ABS(state, Get8BMem, Upd8BMem, LRealOf, FromLReal)
                                   |0x20uy -> state <- SQRT(state, Get4BMem, Upd4BMem, RealOf, FromReal)
                                   |0x21uy -> state <- SQRT(state, Get8BMem, Upd8BMem, LRealOf, FromLReal)
                                   |0x22uy -> state <- LN(state, Get4BMem, Upd4BMem, RealOf, FromReal)
                                   |0x23uy -> state <- LN(state, Get8BMem, Upd8BMem, LRealOf, FromLReal)
                                   |0x24uy -> state <- LOG(state, Get4BMem, Upd4BMem, RealOf, FromReal)
                                   |0x25uy -> state <- LOG(state, Get8BMem, Upd8BMem, LRealOf, FromLReal)
                                   |0x26uy -> state <- EXP(state, Get4BMem, Upd4BMem, RealOf, FromReal)
                                   |0x27uy -> state <- EXP(state, Get8BMem, Upd8BMem, LRealOf, FromLReal)
                                   |0x28uy -> state <- SIN(state, Get4BMem, Upd4BMem, RealOf, FromReal)
                                   |0x29uy -> state <- SIN(state, Get8BMem, Upd8BMem, LRealOf, FromLReal)
                                   |0x2Auy -> state <- COS(state, Get4BMem, Upd4BMem, RealOf, FromReal)
                                   |0x2Buy -> state <- COS(state, Get8BMem, Upd8BMem, LRealOf, FromLReal)
                                   |0x2Cuy -> state <- TAN(state, Get4BMem, Upd4BMem, RealOf, FromReal)
                                   |0x2Duy -> state <- TAN(state, Get8BMem, Upd8BMem, LRealOf, FromLReal)
                                   |0x2Euy -> state <- ASIN(state, Get4BMem, Upd4BMem, RealOf, FromReal)
                                   |0x2Fuy -> state <- ASIN(state, Get8BMem, Upd8BMem, LRealOf, FromLReal)
                                   |0x30uy -> state <- ACOS(state, Get4BMem, Upd4BMem, RealOf, FromReal)
                                   |0x31uy -> state <- ACOS(state, Get8BMem, Upd8BMem, LRealOf, FromLReal)
                                   |0x32uy -> state <- ATAN(state, Get4BMem, Upd4BMem, RealOf, FromReal)
                                   |0x33uy -> state <- ATAN(state, Get8BMem, Upd8BMem, LRealOf, FromLReal)
                                   |0x34uy -> state <- TRUNC(state, Get4BMem, Upd4BMem, RealOf, FromDInt)
                                   |0x35uy -> state <- TRUNC(state, Get8BMem, Upd8BMem, LRealOf, FromLInt)
                                   |0x36uy -> state <- ROUND(state, Get4BMem, Upd4BMem, RealOf, FromDInt)
                                   |0x37uy -> state <- ROUND(state, Get8BMem, Upd8BMem, LRealOf, FromLInt)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x07uy -> match ic with
                                   |a when (a &&& 0x80uy) = 0uy  -> state <- MMV1(state, (uint16 a))
                                   |b when (b &&& 0x80uy) <> 0uy -> state <- MMV2(state, uint16 <| (b &&& 0x7Fuy))
                                   |_                            -> flaga <- false
                                                                    loop  <- false
                  |0x08uy -> match ic with
                                   |a when (a &&& 0x0Fuy) = 4uy  -> state <- AND(state, Get8BMem, Upd8BMem, LWordOf, FromLWord,(int <| (a &&& 0xF0uy))/16)
                                   |b when (b &&& 0x0Fuy) = 3uy  -> state <- AND(state, Get4BMem, Upd4BMem, DWordOf, FromDWord,(int <| (b &&& 0xF0uy))/16)
                                   |c when (c &&& 0x0Fuy) = 2uy  -> state <- AND(state, Get2BMem, Upd2BMem, WordOf, FromWord,(int <| (c &&& 0xF0uy))/16)
                                   |d when (d &&& 0x0Fuy) = 1uy  -> state <- AND(state, Get1BMem, Upd1BMem, ByteOf, FromByte,(int <| (d &&& 0xF0uy))/16)
                                   |e when (e &&& 0x0Fuy) = 0uy  -> state <- AND(state, Get1BMem, Upd1BMem, BoolOf, FromBool,(int <| (e &&& 0xF0uy))/16)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x09uy -> match ic with
                                   |a when (a &&& 0x0Fuy) = 4uy  -> state <- OR(state, Get8BMem, Upd8BMem, LWordOf, FromLWord,(int <| (a &&& 0xF0uy))/16)
                                   |b when (b &&& 0x0Fuy) = 3uy  -> state <- OR(state, Get4BMem, Upd4BMem, DWordOf, FromDWord,(int <| (b &&& 0xF0uy))/16)
                                   |c when (c &&& 0x0Fuy) = 2uy  -> state <- OR(state, Get2BMem, Upd2BMem, WordOf, FromWord,(int <| (c &&& 0xF0uy))/16)
                                   |d when (d &&& 0x0Fuy) = 1uy  -> state <- OR(state, Get1BMem, Upd1BMem, ByteOf, FromByte,(int <| (d &&& 0xF0uy))/16)
                                   |e when (e &&& 0x0Fuy) = 0uy  -> state <- OR(state, Get1BMem, Upd1BMem, BoolOf, FromBool,(int <| (e &&& 0xF0uy))/16)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x0Auy -> match ic with
                                   |a when (a &&& 0x0Fuy) = 5uy  -> state <- XOR(state, Get8BMem, Upd8BMem, LWordOf, FromLWord,(int <| (a &&& 0xF0uy))/16)
                                   |b when (b &&& 0x0Fuy) = 4uy  -> state <- XOR(state, Get4BMem, Upd4BMem, DWordOf, FromDWord,(int <| (b &&& 0xF0uy))/16)
                                   |c when (c &&& 0x0Fuy) = 2uy  -> state <- XOR(state, Get2BMem, Upd2BMem, WordOf, FromWord,(int <| (c &&& 0xF0uy))/16)
                                   |d when (d &&& 0x0Fuy) = 1uy  -> state <- XOR(state, Get1BMem, Upd1BMem, ByteOf, FromByte,(int <| (d &&& 0xF0uy))/16)
                                   |e when (e &&& 0x0Fuy) = 0uy  -> state <- XOR(state, Get1BMem, Upd1BMem, BoolOf, FromBool,(int <| (e &&& 0xF0uy))/16)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x0Buy -> match ic with
                                   |0x01uy -> state <- SHL(state, Get1BMem, Upd1BMem, ByteOf, FromByte)
                                   |0x02uy -> state <- SHL(state, Get2BMem, Upd2BMem, WordOf, FromWord)
                                   |0x03uy -> state <- SHL(state, Get4BMem, Upd4BMem, DWordOf, FromDWord)
                                   |0x04uy -> state <- SHL(state, Get8BMem, Upd8BMem, LWordOf, FromLWord)
                                   |0x11uy -> state <- SHR(state, Get1BMem, Upd1BMem, ByteOf, FromByte)
                                   |0x12uy -> state <- SHR(state, Get2BMem, Upd2BMem, WordOf, FromWord)
                                   |0x13uy -> state <- SHR(state, Get4BMem, Upd4BMem, DWordOf, FromDWord)
                                   |0x14uy -> state <- SHR(state, Get8BMem, Upd8BMem, LWordOf, FromLWord)
                                   |0x21uy -> state <- ROL(state, Get1BMem, Upd1BMem, ByteOf, FromByte)
                                   |0x22uy -> state <- ROL(state, Get2BMem, Upd2BMem, WordOf, FromWord)
                                   |0x23uy -> state <- ROL(state, Get4BMem, Upd4BMem, DWordOf, FromDWord)
                                   |0x24uy -> state <- ROL(state, Get8BMem, Upd8BMem, LWordOf, FromLWord)
                                   |0x31uy -> state <- ROR(state, Get1BMem, Upd1BMem, ByteOf, FromByte)
                                   |0x32uy -> state <- ROR(state, Get2BMem, Upd2BMem, WordOf, FromWord)
                                   |0x33uy -> state <- ROR(state, Get4BMem, Upd4BMem, DWordOf, FromDWord)
                                   |0x34uy -> state <- ROR(state, Get8BMem, Upd8BMem, LWordOf, FromLWord)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x0Cuy -> match ic with
                                   |0x00uy -> state <- SEL(state, Get1BMem, Upd1BMem, BoolOf, FromBool)
                                   |0x01uy -> state <- SEL(state, Get1BMem, Upd1BMem, SIntOf, FromSInt)
                                   |0x02uy -> state <- SEL(state, Get2BMem, Upd2BMem, IntOf, FromInt)
                                   |0x03uy -> state <- SEL(state, Get4BMem, Upd4BMem, DIntOf, FromDInt)
                                   |0x04uy -> state <- SEL(state, Get8BMem, Upd8BMem, LIntOf, FromLInt)
                                   |0x05uy -> state <- SEL(state, Get1BMem, Upd1BMem, ByteOf, FromByte)
                                   |0x06uy -> state <- SEL(state, Get2BMem, Upd2BMem, WordOf, FromWord)
                                   |0x07uy -> state <- SEL(state, Get4BMem, Upd4BMem, DWordOf, FromDWord)
                                   |0x08uy -> state <- SEL(state, Get8BMem, Upd8BMem, LWordOf, FromLWord)
                                   |0x09uy -> state <- SEL(state, Get4BMem, Upd4BMem, RealOf, FromReal)
                                   |0x0Auy -> state <- SEL(state, Get8BMem, Upd8BMem, LRealOf, FromLReal)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x0Duy -> match ic with
                                   |0x00uy -> state <- LIMIT(state, Get1BMem, Upd1BMem, BoolOf, FromBool)
                                   |0x01uy -> state <- LIMIT(state, Get1BMem, Upd1BMem, SIntOf, FromSInt)
                                   |0x02uy -> state <- LIMIT(state, Get2BMem, Upd2BMem, IntOf, FromInt)
                                   |0x03uy -> state <- LIMIT(state, Get4BMem, Upd4BMem, DIntOf, FromDInt)
                                   |0x04uy -> state <- LIMIT(state, Get8BMem, Upd8BMem, LIntOf, FromLInt)
                                   |0x05uy -> state <- LIMIT(state, Get1BMem, Upd1BMem, ByteOf, FromByte)
                                   |0x06uy -> state <- LIMIT(state, Get2BMem, Upd2BMem, WordOf, FromWord)
                                   |0x07uy -> state <- LIMIT(state, Get4BMem, Upd4BMem, DWordOf, FromDWord)
                                   |0x08uy -> state <- LIMIT(state, Get8BMem, Upd8BMem, LWordOf, FromLWord)
                                   |0x09uy -> state <- LIMIT(state, Get4BMem, Upd4BMem, RealOf, FromReal)
                                   |0x0Auy -> state <- LIMIT(state, Get8BMem, Upd8BMem, LRealOf, FromLReal)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x0Euy -> match ic with
                                   |0x00uy -> state <- MAX(state, Get1BMem, Upd1BMem, BoolOf, FromBool)
                                   |0x01uy -> state <- MAX(state, Get1BMem, Upd1BMem, SIntOf, FromSInt)
                                   |0x02uy -> state <- MAX(state, Get2BMem, Upd2BMem, IntOf, FromInt)
                                   |0x03uy -> state <- MAX(state, Get4BMem, Upd4BMem, DIntOf, FromDInt)
                                   |0x04uy -> state <- MAX(state, Get8BMem, Upd8BMem, LIntOf, FromLInt)
                                   |0x05uy -> state <- MAX(state, Get1BMem, Upd1BMem, ByteOf, FromByte)
                                   |0x06uy -> state <- MAX(state, Get2BMem, Upd2BMem, WordOf, FromWord)
                                   |0x07uy -> state <- MAX(state, Get4BMem, Upd4BMem, DWordOf, FromDWord)
                                   |0x08uy -> state <- MAX(state, Get8BMem, Upd8BMem, LWordOf, FromLWord)
                                   |0x09uy -> state <- MAX(state, Get4BMem, Upd4BMem, RealOf, FromReal)
                                   |0x0Auy -> state <- MAX(state, Get8BMem, Upd8BMem, LRealOf, FromLReal)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x0Fuy -> match ic with
                                   |0x00uy -> state <- MIN(state, Get1BMem, Upd1BMem, BoolOf, FromBool)
                                   |0x01uy -> state <- MIN(state, Get1BMem, Upd1BMem, SIntOf, FromSInt)
                                   |0x02uy -> state <- MIN(state, Get2BMem, Upd2BMem, IntOf, FromInt)
                                   |0x03uy -> state <- MIN(state, Get4BMem, Upd4BMem, DIntOf, FromDInt)
                                   |0x04uy -> state <- MIN(state, Get8BMem, Upd8BMem, LIntOf, FromLInt)
                                   |0x05uy -> state <- MIN(state, Get1BMem, Upd1BMem, ByteOf, FromByte)
                                   |0x06uy -> state <- MIN(state, Get2BMem, Upd2BMem, WordOf, FromWord)
                                   |0x07uy -> state <- MIN(state, Get4BMem, Upd4BMem, DWordOf, FromDWord)
                                   |0x08uy -> state <- MIN(state, Get8BMem, Upd8BMem, LWordOf, FromLWord)
                                   |0x09uy -> state <- MIN(state, Get4BMem, Upd4BMem, RealOf, FromReal)
                                   |0x0Auy -> state <- MIN(state, Get8BMem, Upd8BMem, LRealOf, FromLReal)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x10uy -> match ic with
                                   |0x00uy -> state <- GT(state, Get1BMem, BoolOf)
                                   |0x01uy -> state <- GT(state, Get1BMem, SIntOf)
                                   |0x02uy -> state <- GT(state, Get2BMem, IntOf)
                                   |0x03uy -> state <- GT(state, Get4BMem, DIntOf)
                                   |0x04uy -> state <- GT(state, Get8BMem, LIntOf)
                                   |0x05uy -> state <- GT(state, Get1BMem, ByteOf)
                                   |0x06uy -> state <- GT(state, Get2BMem, WordOf)
                                   |0x07uy -> state <- GT(state, Get4BMem, DWordOf)
                                   |0x08uy -> state <- GT(state, Get8BMem, LWordOf)
                                   |0x09uy -> state <- GT(state, Get4BMem, RealOf)
                                   |0x0Auy -> state <- GT(state, Get8BMem, LRealOf)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x11uy -> match ic with
                                   |0x00uy -> state <- GE(state, Get1BMem, BoolOf)
                                   |0x01uy -> state <- GE(state, Get1BMem, SIntOf)
                                   |0x02uy -> state <- GE(state, Get2BMem, IntOf)
                                   |0x03uy -> state <- GE(state, Get4BMem, DIntOf)
                                   |0x04uy -> state <- GE(state, Get8BMem, LIntOf)
                                   |0x05uy -> state <- GE(state, Get1BMem, ByteOf)
                                   |0x06uy -> state <- GE(state, Get2BMem, WordOf)
                                   |0x07uy -> state <- GE(state, Get4BMem, DWordOf)
                                   |0x08uy -> state <- GE(state, Get8BMem, LWordOf)
                                   |0x09uy -> state <- GE(state, Get4BMem, RealOf)
                                   |0x0Auy -> state <- GE(state, Get8BMem, LRealOf)
                                   |0x0Buy -> state <- GE(state, Get4BMem, TimeOf)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x12uy -> match ic with
                                   |0x00uy -> state <- EQ(state, Get1BMem, BoolOf)
                                   |0x01uy -> state <- EQ(state, Get1BMem, SIntOf)
                                   |0x02uy -> state <- EQ(state, Get2BMem, IntOf)
                                   |0x03uy -> state <- EQ(state, Get4BMem, DIntOf)
                                   |0x04uy -> state <- EQ(state, Get8BMem, LIntOf)
                                   |0x05uy -> state <- EQ(state, Get1BMem, ByteOf)
                                   |0x06uy -> state <- EQ(state, Get2BMem, WordOf)
                                   |0x07uy -> state <- EQ(state, Get4BMem, DWordOf)
                                   |0x08uy -> state <- EQ(state, Get8BMem, LWordOf)
                                   |0x09uy -> state <- EQ(state, Get4BMem, RealOf)
                                   |0x0Auy -> state <- EQ(state, Get8BMem, LRealOf)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x13uy -> match ic with
                                   |0x00uy -> state <- LE(state, Get1BMem, BoolOf)
                                   |0x01uy -> state <- LE(state, Get1BMem, SIntOf)
                                   |0x02uy -> state <- LE(state, Get2BMem, IntOf)
                                   |0x03uy -> state <- LE(state, Get4BMem, DIntOf)
                                   |0x04uy -> state <- LE(state, Get8BMem, LIntOf)
                                   |0x05uy -> state <- LE(state, Get1BMem, ByteOf)
                                   |0x06uy -> state <- LE(state, Get2BMem, WordOf)
                                   |0x07uy -> state <- LE(state, Get4BMem, DWordOf)
                                   |0x08uy -> state <- LE(state, Get8BMem, LWordOf)
                                   |0x09uy -> state <- LE(state, Get4BMem, RealOf)
                                   |0x0Auy -> state <- LE(state, Get8BMem, LRealOf)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x14uy -> match ic with
                                   |0x00uy -> state <- LT(state, Get1BMem, BoolOf)
                                   |0x01uy -> state <- LT(state, Get1BMem, SIntOf)
                                   |0x02uy -> state <- LT(state, Get2BMem, IntOf)
                                   |0x03uy -> state <- LT(state, Get4BMem, DIntOf)
                                   |0x04uy -> state <- LT(state, Get8BMem, LIntOf)
                                   |0x05uy -> state <- LT(state, Get1BMem, ByteOf)
                                   |0x06uy -> state <- LT(state, Get2BMem, WordOf)
                                   |0x07uy -> state <- LT(state, Get4BMem, DWordOf)
                                   |0x08uy -> state <- LT(state, Get8BMem, LWordOf)
                                   |0x09uy -> state <- LT(state, Get4BMem, RealOf)
                                   |0x0Auy -> state <- LT(state, Get8BMem, LRealOf)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x15uy -> match ic with
                                   |0x00uy -> state <- NE(state, Get1BMem, BoolOf)
                                   |0x01uy -> state <- NE(state, Get1BMem, SIntOf)
                                   |0x02uy -> state <- NE(state, Get2BMem, IntOf)
                                   |0x03uy -> state <- NE(state, Get4BMem, DIntOf)
                                   |0x04uy -> state <- NE(state, Get8BMem, LIntOf)
                                   |0x05uy -> state <- NE(state, Get1BMem, ByteOf)
                                   |0x06uy -> state <- NE(state, Get2BMem, WordOf)
                                   |0x07uy -> state <- NE(state, Get4BMem, DWordOf)
                                   |0x08uy -> state <- NE(state, Get8BMem, LWordOf)
                                   |0x09uy -> state <- NE(state, Get4BMem, RealOf)
                                   |0x0Auy -> state <- NE(state, Get8BMem, LRealOf)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x19uy -> match ic with
                                   |0x00uy -> state <- INT_TO_REAL(state)
                                   |0x01uy -> state <- DINT_TO_REAL(state)
                                   |0x0Auy -> state <- REAL_TO_INT(state)
                                   |0x0Buy -> state <- INT_TO_BOOL(state)
                                   |0x0Cuy -> state <- INT_TO_DINT(state)
                                   |0x0Duy -> state <- REAL_TO_LREAL(state)
                                   |0x0Euy -> state <- LREAL_TO_REAL(state)
                                   |0x10uy -> state <- DWORD_TO_REAL(state)
                                   |0x11uy -> state <- REAL_TO_DWORD(state)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x1Cuy -> match ic with
                                   |0x00uy -> state <- JMP(state)
                                   |0x01uy -> state <- JNZ(state)
                                   |0x02uy -> state <- JZ(state)
                                   |0x03uy -> state <- JR(state)
                                   |0x04uy -> state <- JRN(state)
                                   |0x05uy -> state <- JRZ(state)
                                   |0x13uy -> state <- RETURN(state)
                                   |0x15uy -> state <- MCD(state)
                                   |0x16uy -> state <- CALB(state)
                                   |0x17uy -> state <- CUR_TIME(state)
                                   |0x1Duy -> state <- TRML(state)
                                   |0x1Fuy -> state <- MEMCP(state)
                                   |0x23uy -> state <- FPAT(state)
                                   |0x24uy -> state <- AURD(state)
                                   |0x25uy -> state <- AUWR(state)
                                   |0x26uy -> state <- AORD(state)
                                   |0x27uy -> state <- AOWR(state)
                                   |0x28uy -> state <- ATRD(state)
                                   |0x29uy -> state <- ATWR(state)
                                   |0x2Auy -> state <- AFRD(state)
                                   |0x2Buy -> state <- AFWR(state)
                                   |0x2Cuy -> state <- AQRD(state)
                                   |0x2Duy -> state <- AQWR(state)
                                   |0x2Euy -> state <- CEAC(state)
                                   |0x34uy -> state <- GARD(state)
                                   |0x35uy -> state <- GAWR(state)
                                   |0x3Auy -> state <- FINIT(state)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |0x1Euy -> match ic with
                                   |0x00uy -> state <- DPSTO(state)
                                   |0x01uy -> state <- DPRDL(state)
                                   |0x02uy -> state <- DPWRL(state)
                                   |_      -> flaga <- false
                                              loop  <- false
                  |_ -> flaga <- false
                        loop  <- false


            let (newCodeMemoryStorage,newDataMemoryStorage,newCodeStack,newDataStack,newCodeReg,newDataReg,newExcState,newFlags) = state
            codeMemoryStorage <- newCodeMemoryStorage
            dataMemoryStorage <- newDataMemoryStorage
            codeStack <- newCodeStack
            dataStack <- newDataStack
            codeReg <- newCodeReg
            dataReg <- newDataReg
            flags <- newFlags

        if (flaga) then
            //printfn "\nWykonuję własne sprawy\n"
            let timeLeft : int = 2000 - int((DateTime.Now - startCycle).TotalMilliseconds)
            printfn "Zostało: %A" timeLeft
            if (timeLeft > 0) then
                Thread.Sleep(timeLeft)
            let Convert_ToAddress(_p : string) : Address =
#if VM_ADDRESSING_32
                Convert.ToUInt32(_p)
#else
                Convert.ToUInt16(_p)
#endif
            if Console.KeyAvailable then match Console.ReadKey().KeyChar with 
                                         | 'u' -> printfn "Podaj address zmiennej, typ i nową wartość:"
                                                  let kv = Console.ReadLine()
                                                  let pary = kv.Split(' ')
                                                  match pary.[1] with
                                                  | "BYTE"  -> dataMemoryStorage <- Upd1BMem(Convert_ToAddress(pary.[0]), dataMemoryStorage, FromByte(  BYTE(Convert.ToByte(pary.[2]))))
                                                  | "SINT"  -> dataMemoryStorage <- Upd1BMem(Convert_ToAddress(pary.[0]), dataMemoryStorage, FromSInt(  SINT(Convert.ToSByte(pary.[2]))))
                                                  | "INT"   -> dataMemoryStorage <- Upd2BMem(Convert_ToAddress(pary.[0]), dataMemoryStorage, FromInt(   INT(Convert.ToInt16(pary.[2]))))
                                                  | "WORD"  -> dataMemoryStorage <- Upd2BMem(Convert_ToAddress(pary.[0]), dataMemoryStorage, FromWord(  WORD(Convert.ToUInt16(pary.[2]))))
                                                  | "DINT"  -> dataMemoryStorage <- Upd4BMem(Convert_ToAddress(pary.[0]), dataMemoryStorage, FromDInt(  DINT(Convert.ToInt32(pary.[2]))))
                                                  | "DWORD" -> dataMemoryStorage <- Upd4BMem(Convert_ToAddress(pary.[0]), dataMemoryStorage, FromDWord( DWORD(Convert.ToUInt32(pary.[2]))))
                                                  | "LINT"  -> dataMemoryStorage <- Upd8BMem(Convert_ToAddress(pary.[0]), dataMemoryStorage, FromLInt(  LINT(Convert.ToInt64(pary.[2]))))
                                                  | "LWORD" -> dataMemoryStorage <- Upd8BMem(Convert_ToAddress(pary.[0]), dataMemoryStorage, FromLWord( LWORD(Convert.ToUInt64(pary.[2]))))
                                                  | "REAL"  -> dataMemoryStorage <- Upd4BMem(Convert_ToAddress(pary.[0]), dataMemoryStorage, FromReal(  REAL(Convert.ToSingle(pary.[2]))))
                                                  | "LREAL" -> dataMemoryStorage <- Upd8BMem(Convert_ToAddress(pary.[0]), dataMemoryStorage, FromLReal( LREAL(Convert.ToDouble(pary.[2]))))
                                                  | _       -> ()
                                         |_    -> flaga <- false
                                                  printfn "Przerwano wykonywanie zadania\n"
            printfn "Skończyłem wykonywać własne sprawy"
        else
            printfn "Błędna instrukcja!"
    
        if(display_memory = true) then
                    let (cm,dm,cs,ds,cr,dr,ex,flg) = state
                    //printfn "\n Pamięć kodu:" 
                    //printfn "%A" <| Array.take 50 cm
                    printfn "Pamięć danych:"
                    let monitoredData = Array.take 10 dm
                    printByteArrayAsHex monitoredData

    printfn "\nKONIEC PROGRAMU"
    
    let znak =  Console.ReadLine()
    0 // return an integer exit code
