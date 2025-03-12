import SwiftUI

struct ControlView: View {
    @EnvironmentObject var bluetoothManager: BluetoothManager
    @State private var speedLimit: Double = 25.0
    @State private var isSpeedLimitEnabled = true
    
    var body: some View {
        VStack(spacing: 30) {
            // 速度限制控制
            VStack {
                Toggle("启用速度限制", isOn: $isSpeedLimitEnabled)
                    .padding()
                
                if isSpeedLimitEnabled {
                    VStack {
                        Text("速度限制: \(Int(speedLimit)) km/h")
                        Slider(value: $speedLimit, in: 0...30, step: 1)
                            .padding(.horizontal)
                    }
                }
            }
            .padding()
            .background(Color.gray.opacity(0.1))
            .cornerRadius(10)
            
            // 紧急制动按钮
            Button(action: {
                bluetoothManager.setSpeed(0)
            }) {
                Text("紧急制动")
                    .font(.headline)
                    .foregroundColor(.white)
                    .frame(maxWidth: .infinity)
                    .padding()
                    .background(Color.red)
                    .cornerRadius(10)
            }
            .padding(.horizontal)
            
            // 驾驶模式选择
            VStack {
                Text("驾驶模式")
                    .font(.headline)
                    .padding(.bottom)
                
                HStack(spacing: 20) {
                    DrivingModeButton(mode: "节能", icon: "leaf.fill", color: .green)
                    DrivingModeButton(mode: "标准", icon: "car.fill", color: .blue)
                    DrivingModeButton(mode: "运动", icon: "flame.fill", color: .orange)
                }
            }
            .padding()
            .background(Color.gray.opacity(0.1))
            .cornerRadius(10)
            
            Spacer()
        }
        .padding()
    }
}

struct DrivingModeButton: View {
    let mode: String
    let icon: String
    let color: Color
    
    var body: some View {
        Button(action: {
            // 实现驾驶模式切换逻辑
        }) {
            VStack {
                Image(systemName: icon)
                    .font(.system(size: 30))
                Text(mode)
                    .font(.caption)
            }
            .foregroundColor(color)
            .frame(width: 80, height: 80)
            .background(color.opacity(0.1))
            .cornerRadius(10)
        }
    }
}

struct ControlView_Previews: PreviewProvider {
    static var previews: some View {
        ControlView()
            .environmentObject(BluetoothManager())
    }
} 