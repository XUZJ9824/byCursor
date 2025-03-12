import SwiftUI

struct DashboardView: View {
    @EnvironmentObject var bluetoothManager: BluetoothManager
    
    var body: some View {
        VStack(spacing: 20) {
            // 连接状态
            HStack {
                Image(systemName: bluetoothManager.isConnected ? "bluetooth.connected" : "bluetooth")
                    .foregroundColor(bluetoothManager.isConnected ? .blue : .gray)
                Text(bluetoothManager.isConnected ? "已连接" : "未连接")
                    .foregroundColor(bluetoothManager.isConnected ? .blue : .gray)
            }
            .padding()
            
            // 电池状态
            BatteryIndicatorView(percentage: bluetoothManager.scooterBattery)
                .frame(height: 40)
                .padding()
            
            // 速度表
            SpeedometerView(speed: bluetoothManager.currentSpeed)
                .frame(height: 200)
            
            // 总里程
            VStack {
                Text("总里程")
                    .font(.headline)
                Text(String(format: "%.1f km", bluetoothManager.totalDistance))
                    .font(.title)
                    .bold()
            }
            .padding()
            
            // 锁定状态
            Button(action: {
                bluetoothManager.toggleLock()
            }) {
                HStack {
                    Image(systemName: bluetoothManager.isScooterLocked ? "lock.fill" : "lock.open.fill")
                    Text(bluetoothManager.isScooterLocked ? "已锁定" : "已解锁")
                }
                .font(.headline)
                .foregroundColor(.white)
                .padding()
                .background(bluetoothManager.isScooterLocked ? Color.red : Color.green)
                .cornerRadius(10)
            }
            
            Spacer()
        }
        .padding()
    }
}

struct BatteryIndicatorView: View {
    let percentage: Int
    
    var body: some View {
        HStack {
            Image(systemName: "battery.100")
                .foregroundColor(percentage > 20 ? .green : .red)
            Text("\(percentage)%")
                .font(.headline)
        }
    }
}

struct SpeedometerView: View {
    let speed: Double
    
    var body: some View {
        VStack {
            Text("当前速度")
                .font(.headline)
            Text(String(format: "%.1f", speed))
                .font(.system(size: 60, weight: .bold))
            Text("km/h")
                .font(.subheadline)
        }
    }
}

struct DashboardView_Previews: PreviewProvider {
    static var previews: some View {
        DashboardView()
            .environmentObject(BluetoothManager())
    }
} 