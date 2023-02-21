//
//  DaneST.swift
//  CPDevVM
//
//  Created by Bartek Wójcik on 16/06/2022.
//

import SwiftUI
import Foundation

struct DaneST: Identifiable{
    var id = UUID()
    var nazwa: String
    var wartosc: String
    var address: UInt16
}
struct DaneINT: Identifiable{
    var id = UUID()
    var nazwa: String
    var wartosc: Int16
    var address: UInt16
}
struct DaneBool: Identifiable{
    var id = UUID()
    var nazwa: String
    var wartosc: Bool
    var address: UInt16
}

class Parser : NSObject, XMLParserDelegate {

    var boolowskie : [DaneBool] = []
    var intowskie : [DaneINT] = []
    var stringowskie : [DaneST] = []
    
    func parser(
        _ parser: XMLParser,
        didStartElement elementName: String,
        namespaceURI: String?,
        qualifiedName qName: String?,
        attributes attributeDict: [String : String] = [:]
    ) {
        if (elementName == "VAR"){
            if(attributeDict["Type"] == "BOOL"){
                let obj = DaneBool(nazwa: attributeDict["LName"] ?? "", wartosc: false,
                    address: UInt16(attributeDict["Addr"] ?? "0") ?? 0)
                boolowskie.append(obj)
            }
            if(attributeDict["Type"] == "INT"){
                let obj = DaneINT(nazwa: attributeDict["LName"] ?? "", wartosc: 0,
                    address: UInt16(attributeDict["Addr"] ?? "0") ?? 0)
                intowskie.append(obj)
            }
            if(attributeDict["Type"] == "STRING"){
                let obj = DaneST(nazwa: attributeDict["LName"] ?? "", wartosc: "",
                    address: UInt16(attributeDict["Addr"] ?? "0") ?? 0)
                stringowskie.append(obj)
            }

        }
    }
}

//let pathdcp = "http://127.0.0.1:8080/WeJeStSt/WeJeStSt.dcp"
func parsing(_ path: String) -> Parser{
    let url = URL(string: path)
    let parser = Parser()
    let xmlParser = XMLParser(contentsOf: url!)
    xmlParser!.delegate = parser
    xmlParser!.parse()
    print(" \n")
    return parser
}


var testDataST: [DaneST] = []
var testDataINT32: [DaneINT] = [
    //DaneINT(nazwa: "x1", wartosc: 32,address: 0),
    //DaneINT(nazwa: "x2", wartosc: 21,address: 1),
    //DaneINT32(nazwa: "x3", wartosc: 99,address: 2),
    ]



 
let testDataBool: [DaneBool] = [
    //DaneBool(nazwa: "temp", wartosc: false, address: 0),
    //DaneBool(nazwa: "temp", wartosc: false, address: 0),
    //DaneBool(nazwa: "temp", wartosc: false, address: 0),
    //DaneBool(nazwa: "temp", wartosc: false, address: 0),
]




func zmiana_bool_true(_ wartosc: DaneBool) -> Void{
    setBool__(wartosc.address, UInt8(wartosc.wartosc ? 0 : 1)) //Neg
}
func zmiana_int_plus(_ wartosc: DaneINT) -> Void{
    var temp: Int16 = 0
    temp = wartosc.wartosc + 1
    setInt__(wartosc.address, temp)
}
func zmiana_int_plus5(_ wartosc: DaneINT) -> Void{
    var temp: Int16 = 0
    temp = wartosc.wartosc + 5
    setInt__(wartosc.address, temp)
}
func zmiana_int_plus10(_ wartosc: DaneINT) -> Void{
    var temp: Int16 = 0
    temp = wartosc.wartosc + 10
    setInt__(wartosc.address, temp)
}
func zmiana_int_plus50(_ wartosc: DaneINT) -> Void{
    var temp: Int16 = 0
    temp = wartosc.wartosc + 50
    setInt__(wartosc.address, temp)
}
func zmiana_int_plus100(_ wartosc: DaneINT) -> Void{
    var temp: Int16 = 0
    temp = wartosc.wartosc + 100
    setInt__(wartosc.address, temp)
}
func zmiana_int_minus(_ wartosc: DaneINT) -> Void{
    var temp: Int16 = 0
    temp = wartosc.wartosc - 1
    setInt__(wartosc.address, temp)
}
func zmiana_int_minus5(_ wartosc: DaneINT) -> Void{
    var temp: Int16 = 0
    temp = wartosc.wartosc - 5
    setInt__(wartosc.address, temp)
}
func zmiana_int_minus10(_ wartosc: DaneINT) -> Void{
    var temp: Int16 = 0
    temp = wartosc.wartosc - 10
    setInt__(wartosc.address, temp)
}
func zmiana_int_minus50(_ wartosc: DaneINT) -> Void{
    var temp: Int16 = 0
    temp = wartosc.wartosc - 50
    setInt__(wartosc.address, temp)
}
func zmiana_int_minus100(_ wartosc: DaneINT) -> Void{
    var temp: Int16 = 0
    temp = wartosc.wartosc - 100
    setInt__(wartosc.address, temp)
}


