import asyncio
from bleak import BleakClient
from pynput.mouse import Button, Controller as MouseController
from pynput.keyboard import Controller as KeyboardController, Key

# --- Configuration ---
CHAR_UUID = "0000ffe1-0000-1000-8000-00805f9b34fb"
HM10_ADDRESS = "98:7B:F3:60:86:D2"

mouse = MouseController()
keyboard = KeyboardController()
buffer = ""

# Track the state of keys to prevent "repeating" press commands
key_states = {
    'w': False, 's': False, 'a': False, 'd': False,
    'up': False, 'down': False, 'left': False, 'right': False
}

last_dig1 = None
last_dig6 = None

def handle_key(key, should_press, state_name):
    """Helper to press or release keys only when the state changes."""
    if should_press and not key_states[state_name]:
        keyboard.press(key)
        key_states[state_name] = True
    elif not should_press and key_states[state_name]:
        keyboard.release(key)
        key_states[state_name] = False

async def notification_handler(sender, data):
    global buffer, last_dig1, last_dig6
    
    try:
        raw_data = data.decode('utf-8', errors='ignore')
        buffer += raw_data

        if '\n' in buffer:
            parts = buffer.split('\n')
            for part in parts[:-1]:
                stripped_data = ''.join(filter(str.isdigit, part.strip()))
                
                if len(stripped_data) == 9:
                    value = int(stripped_data)
                    # Extracting digits based on your original logic
                    dig1 = value // 100000000 % 10 # Mouse Click
                    dig3 = value // 1000000 % 10   # W / S
                    dig4 = value // 100000 % 10    # Up / Down
                    dig5 = value // 10000 % 10     # Left / Right
                    dig6 = value // 1000 % 10      # Number Key
                    dig7 = value // 100 % 10       # A / D
                    dig8 = value // 10 % 10        # Mouse X
                    dig9 = value % 10              # Mouse Y

                    # --- Driving Controls (WASD) ---
                    # Forward/Backward
                    handle_key('w', dig3 < 4, 'w')
                    handle_key('s', dig3 > 6, 's')
                    
                    # Left/Right Steering
                    handle_key('a', dig7 < 4, 'a')
                    handle_key('d', dig7 > 6, 'd')

                    # --- Arrow Keys ---
                    handle_key(Key.up, dig4 < 4, 'up')
                    handle_key(Key.down, dig4 > 6, 'down')
                    handle_key(Key.left, dig5 < 4, 'left')
                    handle_key(Key.right, dig5 > 6, 'right')

                    # --- Mouse Movement ---
                    if dig9 > 6: mouse.move(0, -(dig9 - 6))
                    elif dig9 < 4: mouse.move(0, -(dig9 - 4))
                    if dig8 > 6: mouse.move(dig8 - 6, 0)
                    elif dig8 < 4: mouse.move(dig8 - 4, 0)

                    # --- Clicks & Hotkeys ---
                    if dig1 != last_dig1:
                        if dig1 == 1: # Assuming 1 is press, 0 is release
                            mouse.press(Button.left)
                        else:
                            mouse.release(Button.left)
                        last_dig1 = dig1

                    if dig6 != last_dig6:
                        keyboard.tap(str(dig6))
                        last_dig6 = dig6

            buffer = parts[-1]
    except Exception as e:
        pass # Ignore decoding glitches

async def run_controller():
    print(f"Connecting to HM-10...")
    while True:
        try:
            async with BleakClient(HM10_ADDRESS) as client:
                print("Connected! You can now drive.")
                await client.start_notify(CHAR_UUID, notification_handler)
                while client.is_connected:
                    await asyncio.sleep(1)
        except Exception as e:
            print(f"Link lost. Reconnecting...")
            await asyncio.sleep(2)

if __name__ == "__main__":
    asyncio.run(run_controller())
