//
//  SwiftVMFunc.swift
//  CPDevVM
//
//  Created by Bartek Wójcik on 18/11/2022.
//

import Foundation


class FileDownloader {
    static func loadFileAsync(url: URL, completion: @escaping (String?, Error?) -> Void){
        let documentsUrl =  FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first!
        let destinationUrl = documentsUrl.appendingPathComponent(url.lastPathComponent)
        if FileManager().fileExists(atPath: destinationUrl.path){
            print("File already exists [\(destinationUrl.path)]")
            completion(destinationUrl.path, nil)
        }
        else{
            let session = URLSession(configuration: URLSessionConfiguration.default, delegate: nil, delegateQueue: nil)
            var request = URLRequest(url: url)
            request.httpMethod = "GET"
            let task = session.dataTask(with: request, completionHandler:{
                data, response, error in
                if error == nil{
                    if let response = response as? HTTPURLResponse{
                        if response.statusCode == 200{
                            if let data = data{
                                if let _ = try? data.write(to: destinationUrl, options: Data.WritingOptions.atomic){
                                    completion(destinationUrl.path, error)
                                }
                                else{
                                    completion(destinationUrl.path, error)
                                }
                            }
                            else{
                                completion(destinationUrl.path, error)
                            }
                        }
                    }
                }
                else{
                    completion(destinationUrl.path, error)
                }
            })
            task.resume()
        }
    }
}

//@objc
/*
class DummyClass : NSObject
{
    /* @objc
    static func Swift_VMP_LoadProgramAndData() -> Int{
        return 0
    } */
    @objc
    static func Swift_WMP_PreCycle() -> Void {
        NSLog("Test swift pre cycle")
    }
    
    @objc
    static func Swift_WMP_LoadData(data : Data?) -> Int32 {
        return 0 //ObjC_WMP_LoadData(data)
    }
    
    @objc
    static func Swift_WMP_PostCycle() -> Void {
        
    }
    
    @objc
    static func Swift_WMP_PreNextCommand() -> Void {
        
    }
   
    @objc
    static func Swift_WMP_PostProgram() -> Void {
        
    }
    @objc
    static func Swift_WMP_Debug(_ err: UInt8, Aux aux: UInt16) -> Void {
        
    }
}
*/
