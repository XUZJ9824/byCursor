import Foundation
import CoreBluetooth

class BluetoothManager: NSObject, ObservableObject {
    @Published var isConnected = false
    @Published var scooterBattery: Int = 0
    @Published var currentSpeed: Double = 0.0
    @Published var totalDistance: Double = 0.0
    @Published var isScooterLocked = true
    
    private var centralManager: CBCentralManager?
    private var scooterPeripheral: CBPeripheral?
    
    override init() {
        super.init()
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }
    
    func startScanning() {
        centralManager?.scanForPeripherals(withServices: nil, options: nil)
    }
    
    func stopScanning() {
        centralManager?.stopScan()
    }
    
    func connect(to peripheral: CBPeripheral) {
        scooterPeripheral = peripheral
        centralManager?.connect(peripheral, options: nil)
    }
    
    func disconnect() {
        if let peripheral = scooterPeripheral {
            centralManager?.cancelPeripheralConnection(peripheral)
        }
    }
    
    func toggleLock() {
        isScooterLocked.toggle()
        // 在这里实现向滑板车发送锁定/解锁命令的逻辑
    }
    
    func setSpeed(_ speed: Double) {
        currentSpeed = speed
        // 在这里实现向滑板车发送速度控制命令的逻辑
    }
}

extension BluetoothManager: CBCentralManagerDelegate {
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        switch central.state {
        case .poweredOn:
            startScanning()
        case .poweredOff:
            isConnected = false
        default:
            break
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        // 在这里实现设备发现逻辑
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        isConnected = true
        stopScanning()
    }
    
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        isConnected = false
    }
} 