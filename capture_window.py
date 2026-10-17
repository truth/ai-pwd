import time, subprocess, ctypes
from ctypes import wintypes
from PIL import Image

user32 = ctypes.windll.user32
gdi32 = ctypes.windll.gdi32
user32.SetProcessDPIAware()

class RECT(ctypes.Structure):
    _fields_ = [
        ('left', wintypes.LONG),
        ('top', wintypes.LONG),
        ('right', wintypes.LONG),
        ('bottom', wintypes.LONG)
    ]

class BITMAPINFOHEADER(ctypes.Structure):
    _fields_ = [
        ('biSize', wintypes.DWORD),
        ('biWidth', wintypes.LONG),
        ('biHeight', wintypes.LONG),
        ('biPlanes', wintypes.WORD),
        ('biBitCount', wintypes.WORD),
        ('biCompression', wintypes.DWORD),
        ('biSizeImage', wintypes.DWORD),
        ('biXPelsPerMeter', wintypes.LONG),
        ('biYPelsPerMeter', wintypes.LONG),
        ('biClrUsed', wintypes.DWORD),
        ('biClrImportant', wintypes.DWORD)
    ]

def capture_app_window(exe_path, output_image, extra_args=None, wait_sec=2.5):
    cmd = [exe_path]
    if extra_args:
        cmd.extend(extra_args)
    proc = subprocess.Popen(cmd)
    time.sleep(wait_sec)

    found_hwnd = None
    target_w, target_h = 0, 0

    def enum_windows_callback(hwnd, lparam):
        nonlocal found_hwnd, target_w, target_h
        if user32.IsWindowVisible(hwnd):
            pid = wintypes.DWORD()
            user32.GetWindowThreadProcessId(hwnd, ctypes.byref(pid))
            if pid.value == proc.pid:
                r = RECT()
                user32.GetWindowRect(hwnd, ctypes.byref(r))
                w = r.right - r.left
                h = r.bottom - r.top
                if w > 400 and h > 300:
                    found_hwnd = hwnd
                    target_w = w
                    target_h = h
                    return False
        return True

    CMPFUNC = ctypes.WINFUNCTYPE(ctypes.c_bool, wintypes.HWND, wintypes.LPARAM)
    user32.EnumWindows(CMPFUNC(enum_windows_callback), 0)

    if found_hwnd and target_w > 0 and target_h > 0:
        hdc_screen = user32.GetDC(0)
        hdc_mem = gdi32.CreateCompatibleDC(hdc_screen)
        hbm = gdi32.CreateCompatibleBitmap(hdc_screen, target_w, target_h)
        old_bm = gdi32.SelectObject(hdc_mem, hbm)

        # 2 = PW_RENDERFULLCONTENT
        user32.PrintWindow(found_hwnd, hdc_mem, 2)

        bmi = BITMAPINFOHEADER()
        bmi.biSize = ctypes.sizeof(BITMAPINFOHEADER)
        bmi.biWidth = target_w
        bmi.biHeight = -target_h # top-down
        bmi.biPlanes = 1
        bmi.biBitCount = 32
        bmi.biCompression = 0

        buffer = ctypes.create_string_buffer(target_w * target_h * 4)
        gdi32.GetDIBits(hdc_mem, hbm, 0, target_h, buffer, ctypes.byref(bmi), 0)

        # Cleanup GDI
        gdi32.SelectObject(hdc_mem, old_bm)
        gdi32.DeleteObject(hbm)
        gdi32.DeleteDC(hdc_mem)
        user32.ReleaseDC(0, hdc_screen)

        img = Image.frombuffer('RGBA', (target_w, target_h), buffer, 'raw', 'BGRA', 0, 1)
        # Convert to RGB
        img.convert('RGB').save(output_image)
        print(f"Captured window {target_w}x{target_h} -> {output_image}")
    else:
        print("Window not found")

    proc.terminate()
    proc.wait()

if __name__ == '__main__':
    import sys
    out = sys.argv[1] if len(sys.argv) > 1 else 'screenshot.png'
    extra = sys.argv[2:] if len(sys.argv) > 2 else None
    capture_app_window(r'F:\AI-Projects\ai-pwd\build\Release\ai_pwd.exe', out, extra_args=extra)
