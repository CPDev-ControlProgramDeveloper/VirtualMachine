//
//  ContentView.swift
//  CPDevVM
//
//  Created by Bartek Wójcik on 16/06/2022.
//

import SwiftUI
struct ContentView: View {
    
    
    @State private var urls: String = ""
    @State private var dane: [DaneST] = testDataST
    @State private var dane2: [DaneINT] = testDataINT32
    @State private var dane3: [DaneBool] = testDataBool
    @State private var pathDCP: String = ""
    @State private var pathXCP: String = ""
    var tm = Timer.publish(every: 0.5, on: .main, in: .common).autoconnect()
    @State private var loadedXCP : Bool = false
   
    var body: some View {
        Form{
//-------------------------------------------------------------------------------------------
//Obsługa zmian cyklicznych------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
            Text("").onReceive(tm){
                input in
                if loadedXCP {
                    WM_RunCycle()
                    dane2 = dane2.map({ db in
                        return DaneINT(id: db.id, nazwa: db.nazwa, wartosc: getInt__(db.address), address: db.address)
                    })
                    dane3 = dane3.map({ db in
                        return DaneBool(id: db.id, nazwa: db.nazwa, wartosc: getByte__(db.address) != 0, address: db.address)
                    })
                }
            }
//-------------------------------------------------------------------------------------------
//Przygotowanie plików-----------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
            TextField(text: $urls, prompt: Text("Adres pliku")) {
                    Text("Adres pliku")
                }
            Button(action: {
                Task{
                    let url = URL(string: urls)
                    FileDownloader.loadFileAsync(url: url!) { (path, error) in
                        print("XCP Został zapisany w: \(path!)")
                        pathXCP = path!
                        print(pathXCP)
                    }
                }
            }){
                Text("Pobieranie pliku XCP")
            }
            Button(action: {
                Task{
                    pathDCP = urls
                    print("Wczytano")
                    }
                }){
                    Text("Wczytanie pliku DCP")
                }
            
//-------------------------------------------------------------------------------------------
//Uruchamianie programu----------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
            Button(action: {
                Task{
                    //pathXCP = "/Users/nackin/Desktop/Inżynierka/int/add_int.xcp"
                    //pathDCP = "/Users/nackin/Desktop/Inżynierka/int/add_int.dcp"
                    //print("\n\n\n\n",pathXCP)
                    let cs = (pathXCP as NSString).utf8String
                    VMP_LoadProgramAndData(cs, 0x10000)
                    WM_Initialize(WM_MODE_CONTINUE | WM_MODE_COLDRESTART | WM_MODE_FIRST_START);
                    let w = parsing(pathDCP)
                    loadedXCP = true
                    self.dane = w.stringowskie
                    self.dane2 = w.intowskie
                    self.dane3 = w.boolowskie
                    /* do zatrzymania
                    tm.invalidate()
                    */
                }
            }){
                Text("Uruchom program ")
            }
//-------------------------------------------------------------------------------------------
//WYSWIETLANIE STRING------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

            Text("Zmienne typu String")
                .font(.title)
            List(dane) { dane in
                VStack(alignment: .leading) {
                    HStack(alignment: .top) {
                        Text(dane.nazwa)
                            .font(.headline)
                            .multilineTextAlignment(.leading)
                        Spacer()
                        
                        Text(dane.wartosc)
                            .multilineTextAlignment(.trailing)
                    }
                }
            }
//-------------------------------------------------------------------------------------------
//WYSWIETLANIE INT---------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
            Text("Zmienne typu INT")
                .font(.title)
            List(dane2) { dane2 in
                VStack(alignment: .leading) {
                    HStack(alignment: .top) {
                        Text(dane2.nazwa)
                            .font(.headline)
                            .multilineTextAlignment(.leading)
                        Spacer()
                        Menu("Zmiana wartości") {
                            Button("+1", action: {
                                zmiana_int_plus(dane2)
                            })
                            Button("+5", action: {
                                zmiana_int_plus5(dane2)
                            })
                            Button("+10", action: {
                                zmiana_int_plus10(dane2)
                            })
                            Button("+50", action: {
                                zmiana_int_plus50(dane2)
                            })
                            Button("+100", action: {
                                zmiana_int_plus100(dane2)
                            })
                            Button("-1", action: {
                                zmiana_int_minus(dane2)
                            })
                            Button("-5", action: {
                                zmiana_int_minus5(dane2)
                            })
                            Button("-10", action: {
                                zmiana_int_minus10(dane2)
                            })
                            Button("-50", action: {
                                zmiana_int_minus50(dane2)
                            })
                            Button("-100", action: {
                                zmiana_int_minus100(dane2)
                            })
                        }
                        Spacer()
                        Text(String(dane2.wartosc))
                    }
                }
            }
//-------------------------------------------------------------------------------------------
//WYSWIETLANIE BOOL--------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
            List{
            Text("Zmienne typu BOOL")
                .font(.title)
                ForEach(dane3) { dane3 in
                    VStack(alignment: .leading) {
                        HStack(alignment: .top) {
                            Text(dane3.nazwa)
                                .font(.headline)
                                .multilineTextAlignment(.leading)
                            Spacer()
                            Button(action: {
                                Task{
                                    zmiana_bool_true(dane3)
                                }
                            }){
                                Text("Zmiana wartosci")
                            }
                            Spacer()
                            Text(String(dane3.wartosc ? "true" : "false"))
                        }
                    }}
                Button(action: {
                    Task{
                        WM_Stop()
                        self.dane = []
                        self.dane2 = []
                        self.dane3 = []
                    }
                }){
                    Text("Zatrzymaj program").foregroundColor(.red)
                }
            }
        }.refreshable {
            print("odświezono")
        }
    }
    
struct ContentView_Previews: PreviewProvider {
        static var previews: some View {
            ContentView()
        }
    }
}
