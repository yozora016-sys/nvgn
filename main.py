import hid
import time
import json

VENDOR_ID = 0x0483
PRODUCT_ID = 0x5750

def run_dashboard():
    try:
        mcu = hid.device()
        mcu.open(VENDOR_ID, PRODUCT_ID)
        mcu.set_nonblocking(1)
    except IOError as error_msg:
        print(f" KHÔNG TÌM THẤY STM32: {error_msg}")
        print("Vui lòng cắm cáp USB vào máy tính.")
        return
        
    led = "OFF"
    led_cu=""
    
    print(" TRẠM TRUNG CHUYỂN ĐANG HOẠT ĐỘNG ")
    print(" (Ấn nút trên mạch STM32 để Tạm dừng / Tiếp tục cập nhật dữ liệu)")
    
    try:
        while True:
            
            raw_data = mcu.read(64)
            
            # if raw_data:
            #     print("Dữ liệu thô USB:", raw_data)
            
            # Nếu nhận được dữ liệu (Tức là STM32 đang bật chế độ tự động gửi)
            if raw_data and raw_data[0] == 0x01:
                # 1. BÓC TÁCH DỮ LIỆU TỪ STM32
                nhiet_do = raw_data[1]
                do_am = raw_data[2]
                anh_sang = raw_data[3]

                goi_lenh = [0x00] * 64 
                goi_lenh[0] = 0x01
                if nhiet_do > 30:
                    led = "ON"
                    goi_lenh[1] = 0x01 
                else:
                    led = "OFF"
                    goi_lenh[1] = 0x02 
                    
                if led != led_cu:
                    mcu.write(goi_lenh)
                    led_cu = led
                    print(f" ĐÃ GỬI LỆNH XUỐNG MẠCH: {led}")

                payload = {
                    "timestamp": time.strftime("%Y-%m-%d %H:%M:%S"),
                    "sensor_data": {
                        "temperature": nhiet_do,
                        "humidity": do_am,
                        "light": anh_sang
                    },
                    "actuator_status": {
                        "LED": led
                    }
                }
                
                chuoi_json = json.dumps(payload, indent=4)
                print(f"[{time.strftime('%H:%M:%S')}] Nhiệt độ: {nhiet_do}°C | Độ ẩm: {do_am}% |Ánh sáng: {anh_sang}| LED: {led}")
                
                with open("data.json", "w", encoding="utf-8") as file:
                    file.write(chuoi_json)

            time.sleep(0.05)

    except KeyboardInterrupt:
        print("\nĐã tắt hệ thống.")
    finally:
        mcu.close()

if __name__ == "__main__":
    run_dashboard()