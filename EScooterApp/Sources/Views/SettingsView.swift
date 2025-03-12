import SwiftUI

struct SettingsView: View {
    @EnvironmentObject var bluetoothManager: BluetoothManager
    @State private var isSearching = false
    @State private var showAlert = false
    @State private var alertMessage = ""
    
    var body: some View {
        NavigationView {
            List {
                Section(header: Text("连接")) {
                    HStack {
                        Text("设备状态")
                        Spacer()
                        Text(bluetoothManager.isConnected ? "已连接" : "未连接")
                            .foregroundColor(bluetoothManager.isConnected ? .green : .gray)
                    }
                    
                    Button(action: {
                        if bluetoothManager.isConnected {
                            bluetoothManager.disconnect()
                        } else {
                            isSearching = true
                            bluetoothManager.startScanning()
                        }
                    }) {
                        Text(bluetoothManager.isConnected ? "断开连接" : "搜索设备")
                    }
                }
                
                Section(header: Text("设备信息")) {
                    HStack {
                        Text("电池电量")
                        Spacer()
                        Text("\(bluetoothManager.scooterBattery)%")
                    }
                    
                    HStack {
                        Text("总里程")
                        Spacer()
                        Text(String(format: "%.1f km", bluetoothManager.totalDistance))
                    }
                }
                
                Section(header: Text("安全设置")) {
                    NavigationLink(destination: Text("固件更新")) {
                        Text("固件更新")
                    }
                    
                    NavigationLink(destination: Text("修改密码")) {
                        Text("修改密码")
                    }
                }
                
                Section(header: Text("关于")) {
                    HStack {
                        Text("版本")
                        Spacer()
                        Text("1.0.0")
                    }
                    
                    NavigationLink(destination: Text("使用说明")) {
                        Text("使用说明")
                    }
                    
                    NavigationLink(destination: Text("隐私政策")) {
                        Text("隐私政策")
                    }
                }
            }
            .navigationTitle("设置")
            .alert(isPresented: $showAlert) {
                Alert(title: Text("提示"), message: Text(alertMessage), dismissButton: .default(Text("确定")))
            }
        }
    }
}

struct SettingsView_Previews: PreviewProvider {
    static var previews: some View {
        SettingsView()
            .environmentObject(BluetoothManager())
    }
} 