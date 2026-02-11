#include <windows.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <type_traits>

enum class DataType {
    Char = 0,
    SignedChar,
    UnsignedChar,
    Int8,
    UInt8,
    Int16,
    UInt16,
    Int32,
    UInt32,
    Int64,
    UInt64,
    Float,
    Double,
};

struct MemoryValues {
    volatile char v_char = 65;
    volatile signed char v_schar = 65;
    volatile unsigned char v_uchar = 65;
    volatile int8_t v_i8 = 100;
    volatile uint8_t v_u8 = 100;
    volatile int16_t v_i16 = 1000;
    volatile uint16_t v_u16 = 1000;
    volatile int32_t v_i32 = 1000;
    volatile uint32_t v_u32 = 1000;
    volatile int64_t v_i64 = 1000;
    volatile uint64_t v_u64 = 1000;
    volatile float v_f32 = 1000.0f;
    volatile double v_f64 = 1000.0;
};

struct AppState {
    MemoryValues mem;
    DataType type = DataType::Int32;
    long double minDelta = 1.0L;
    long double maxDelta = 100.0L;
    int intervalMs = 500;
    int autoRemaining = 0;
    std::mt19937_64 rng{std::random_device{}()};
};

static AppState g_state;

enum ControlId {
    IDC_TYPE = 1001,
    IDC_VALUE = 1002,
    IDC_INPUT = 1003,
    IDC_MIN = 1004,
    IDC_MAX = 1005,
    IDC_INTERVAL = 1006,
    IDC_COUNT = 1007,
    IDC_BTN_SET = 1101,
    IDC_BTN_ADD = 1102,
    IDC_BTN_SUB = 1103,
    IDC_BTN_RAND = 1104,
    IDC_BTN_BURST = 1105,
    IDC_BTN_AUTO = 1106,
    IDC_BTN_STOP = 1107,
    IDC_INFO = 1201,
    IDC_STATUS = 1202,
    IDC_ADDR = 1203,
    IDC_BTN_COPY_ADDR = 1204
};

constexpr UINT_PTR kTimerAuto = 1;
constexpr UINT_PTR kTimerWatch = 2;

template <typename T>
T clampToType(long double value) {
    if constexpr (std::is_integral_v<T>) {
        long double low = static_cast<long double>(std::numeric_limits<T>::lowest());
        long double high = static_cast<long double>(std::numeric_limits<T>::max());
        value = std::max(low, std::min(high, value));
        if constexpr (std::is_unsigned_v<T>) {
            return static_cast<T>(std::llround(std::max(0.0L, value)));
        }
        return static_cast<T>(std::llround(value));
    }
    long double low = -static_cast<long double>(std::numeric_limits<T>::max());
    long double high = static_cast<long double>(std::numeric_limits<T>::max());
    value = std::max(low, std::min(high, value));
    return static_cast<T>(value);
}

template <typename T>
long double toLongDouble(volatile T* p) {
    return static_cast<long double>(*p);
}

template <typename T>
void setFromLongDouble(volatile T* p, long double v) {
    *p = clampToType<T>(v);
}

volatile void* selectedPtr() {
    switch (g_state.type) {
        case DataType::Char: return &g_state.mem.v_char;
        case DataType::SignedChar: return &g_state.mem.v_schar;
        case DataType::UnsignedChar: return &g_state.mem.v_uchar;
        case DataType::Int8: return &g_state.mem.v_i8;
        case DataType::UInt8: return &g_state.mem.v_u8;
        case DataType::Int16: return &g_state.mem.v_i16;
        case DataType::UInt16: return &g_state.mem.v_u16;
        case DataType::Int32: return &g_state.mem.v_i32;
        case DataType::UInt32: return &g_state.mem.v_u32;
        case DataType::Int64: return &g_state.mem.v_i64;
        case DataType::UInt64: return &g_state.mem.v_u64;
        case DataType::Float: return &g_state.mem.v_f32;
        case DataType::Double: return &g_state.mem.v_f64;
        default: return &g_state.mem.v_i32;
    }
}

long double getValue() {
    switch (g_state.type) {
        case DataType::Char: return toLongDouble(&g_state.mem.v_char);
        case DataType::SignedChar: return toLongDouble(&g_state.mem.v_schar);
        case DataType::UnsignedChar: return toLongDouble(&g_state.mem.v_uchar);
        case DataType::Int8: return toLongDouble(&g_state.mem.v_i8);
        case DataType::UInt8: return toLongDouble(&g_state.mem.v_u8);
        case DataType::Int16: return toLongDouble(&g_state.mem.v_i16);
        case DataType::UInt16: return toLongDouble(&g_state.mem.v_u16);
        case DataType::Int32: return toLongDouble(&g_state.mem.v_i32);
        case DataType::UInt32: return toLongDouble(&g_state.mem.v_u32);
        case DataType::Int64: return toLongDouble(&g_state.mem.v_i64);
        case DataType::UInt64: return toLongDouble(&g_state.mem.v_u64);
        case DataType::Float: return toLongDouble(&g_state.mem.v_f32);
        case DataType::Double: return toLongDouble(&g_state.mem.v_f64);
        default: return 0.0L;
    }
}

void setValue(long double v) {
    switch (g_state.type) {
        case DataType::Char: setFromLongDouble(&g_state.mem.v_char, v); break;
        case DataType::SignedChar: setFromLongDouble(&g_state.mem.v_schar, v); break;
        case DataType::UnsignedChar: setFromLongDouble(&g_state.mem.v_uchar, v); break;
        case DataType::Int8: setFromLongDouble(&g_state.mem.v_i8, v); break;
        case DataType::UInt8: setFromLongDouble(&g_state.mem.v_u8, v); break;
        case DataType::Int16: setFromLongDouble(&g_state.mem.v_i16, v); break;
        case DataType::UInt16: setFromLongDouble(&g_state.mem.v_u16, v); break;
        case DataType::Int32: setFromLongDouble(&g_state.mem.v_i32, v); break;
        case DataType::UInt32: setFromLongDouble(&g_state.mem.v_u32, v); break;
        case DataType::Int64: setFromLongDouble(&g_state.mem.v_i64, v); break;
        case DataType::UInt64: setFromLongDouble(&g_state.mem.v_u64, v); break;
        case DataType::Float: setFromLongDouble(&g_state.mem.v_f32, v); break;
        case DataType::Double: setFromLongDouble(&g_state.mem.v_f64, v); break;
        default: break;
    }
}

bool isFloatType() {
    return g_state.type == DataType::Float || g_state.type == DataType::Double;
}

std::wstring valueText() {
    std::wostringstream oss;
    if (isFloatType()) {
        oss << std::fixed << std::setprecision(6) << static_cast<double>(getValue());
    } else {
        if (g_state.type == DataType::UInt64 || g_state.type == DataType::UInt32 ||
            g_state.type == DataType::UInt16 || g_state.type == DataType::UInt8 ||
            g_state.type == DataType::UnsignedChar) {
            oss << static_cast<unsigned long long>(getValue());
        } else {
            oss << static_cast<long long>(getValue());
        }
    }
    return oss.str();
}

bool parseNumberFromEdit(HWND hwnd, int id, long double* out) {
    wchar_t buf[128] = {0};
    GetWindowTextW(GetDlgItem(hwnd, id), buf, 127);
    try {
        std::wstring s(buf);
        if (s.empty()) return false;
        *out = std::stold(s);
        return true;
    } catch (...) {
        return false;
    }
}

void setStatus(HWND hwnd, const std::wstring& text) {
    SetWindowTextW(GetDlgItem(hwnd, IDC_STATUS), text.c_str());
}

std::wstring addressText() {
    std::wostringstream oss;
    oss << L"0x" << std::hex << reinterpret_cast<std::uintptr_t>(selectedPtr());
    return oss.str();
}

bool copyTextToClipboard(HWND hwnd, const std::wstring& text) {
    if (!OpenClipboard(hwnd)) return false;
    if (!EmptyClipboard()) {
        CloseClipboard();
        return false;
    }

    const SIZE_T bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!mem) {
        CloseClipboard();
        return false;
    }

    void* dst = GlobalLock(mem);
    if (!dst) {
        GlobalFree(mem);
        CloseClipboard();
        return false;
    }

    std::memcpy(dst, text.c_str(), bytes);
    GlobalUnlock(mem);

    if (!SetClipboardData(CF_UNICODETEXT, mem)) {
        GlobalFree(mem);
        CloseClipboard();
        return false;
    }

    CloseClipboard();
    return true;
}

void refreshUi(HWND hwnd) {
    SetWindowTextW(GetDlgItem(hwnd, IDC_VALUE), valueText().c_str());

    std::wostringstream info;
    info << L"PID: " << GetCurrentProcessId();
    SetWindowTextW(GetDlgItem(hwnd, IDC_INFO), info.str().c_str());
    SetWindowTextW(GetDlgItem(hwnd, IDC_ADDR), addressText().c_str());
}

void applyRandomDelta() {
    long double delta = 0.0L;
    if (isFloatType()) {
        std::uniform_real_distribution<long double> dist(g_state.minDelta, g_state.maxDelta);
        delta = dist(g_state.rng);
    } else {
        long long minI = static_cast<long long>(std::llround(g_state.minDelta));
        long long maxI = static_cast<long long>(std::llround(g_state.maxDelta));
        std::uniform_int_distribution<long long> dist(minI, maxI);
        delta = static_cast<long double>(dist(g_state.rng));
    }
    std::uniform_int_distribution<int> sign(0, 1);
    if (sign(g_state.rng) == 0) delta = -delta;
    setValue(getValue() + delta);
}

void loadConfigFromUi(HWND hwnd) {
    long double minD = 0.0L;
    long double maxD = 0.0L;
    long double interval = 0.0L;
    if (!parseNumberFromEdit(hwnd, IDC_MIN, &minD) ||
        !parseNumberFromEdit(hwnd, IDC_MAX, &maxD) ||
        !parseNumberFromEdit(hwnd, IDC_INTERVAL, &interval)) {
        setStatus(hwnd, L"配置解析失败");
        return;
    }
    if (minD < 0 || maxD < 0 || minD > maxD || interval < 1) {
        setStatus(hwnd, L"配置无效");
        return;
    }
    g_state.minDelta = minD;
    g_state.maxDelta = maxD;
    g_state.intervalMs = static_cast<int>(interval);
}

void onCommand(HWND hwnd, int id) {
    if (id == IDC_TYPE) {
        int idx = static_cast<int>(SendMessageW(GetDlgItem(hwnd, IDC_TYPE), CB_GETCURSEL, 0, 0));
        if (idx >= 0) {
            g_state.type = static_cast<DataType>(idx);
            refreshUi(hwnd);
            setStatus(hwnd, L"已切换类型");
        }
        return;
    }

    if (id == IDC_BTN_STOP) {
        KillTimer(hwnd, kTimerAuto);
        g_state.autoRemaining = 0;
        setStatus(hwnd, L"自动变化已停止");
        return;
    }

    if (id == IDC_BTN_COPY_ADDR) {
        if (copyTextToClipboard(hwnd, addressText())) {
            setStatus(hwnd, L"地址已复制到剪贴板");
        } else {
            setStatus(hwnd, L"复制地址失败");
        }
        return;
    }

    loadConfigFromUi(hwnd);

    if (id == IDC_BTN_RAND) {
        applyRandomDelta();
        refreshUi(hwnd);
        setStatus(hwnd, L"随机变化完成");
        return;
    }

    if (id == IDC_BTN_SET || id == IDC_BTN_ADD || id == IDC_BTN_SUB) {
        long double n = 0.0L;
        if (!parseNumberFromEdit(hwnd, IDC_INPUT, &n)) {
            setStatus(hwnd, L"输入数值无效");
            return;
        }
        long double cur = getValue();
        if (id == IDC_BTN_SET) setValue(n);
        if (id == IDC_BTN_ADD) setValue(cur + n);
        if (id == IDC_BTN_SUB) setValue(cur - n);
        refreshUi(hwnd);
        setStatus(hwnd, L"数值已更新");
        return;
    }

    if (id == IDC_BTN_BURST) {
        long double countN = 0.0L;
        if (!parseNumberFromEdit(hwnd, IDC_COUNT, &countN) || countN < 1) {
            setStatus(hwnd, L"次数无效");
            return;
        }
        int count = static_cast<int>(countN);
        for (int i = 0; i < count; ++i) applyRandomDelta();
        refreshUi(hwnd);
        setStatus(hwnd, L"连变完成");
        return;
    }

    if (id == IDC_BTN_AUTO) {
        long double countN = 0.0L;
        if (!parseNumberFromEdit(hwnd, IDC_COUNT, &countN) || countN < 1) {
            setStatus(hwnd, L"次数无效");
            return;
        }
        g_state.autoRemaining = static_cast<int>(countN);
        SetTimer(hwnd, kTimerAuto, g_state.intervalMs, nullptr);
        setStatus(hwnd, L"自动变化开始");
        return;
    }
}

void createControls(HWND hwnd) {
    HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    auto mk = [&](const wchar_t* cls, const wchar_t* txt, DWORD style, int x, int y, int w, int h, int id) {
        HWND c = CreateWindowExW(0, cls, txt, style, x, y, w, h, hwnd, (HMENU)(INT_PTR)id, nullptr, nullptr);
        SendMessageW(c, WM_SETFONT, (WPARAM)font, TRUE);
        return c;
    };

    mk(L"STATIC", L"类型", WS_CHILD | WS_VISIBLE, 12, 12, 80, 22, -1);
    HWND combo = mk(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL, 80, 10, 170, 320, IDC_TYPE);
    const wchar_t* typeNames[] = {
        L"char", L"signed char", L"unsigned char", L"int8_t", L"uint8_t", L"int16_t",
        L"uint16_t", L"int32_t", L"uint32_t", L"int64_t", L"uint64_t", L"float", L"double"
    };
    for (const auto* n : typeNames) SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)n);
    SendMessageW(combo, CB_SETCURSEL, (WPARAM)static_cast<int>(g_state.type), 0);

    mk(L"STATIC", L"当前值", WS_CHILD | WS_VISIBLE, 280, 12, 80, 22, -1);
    mk(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY, 340, 10, 170, 24, IDC_VALUE);

    mk(L"STATIC", L"输入值", WS_CHILD | WS_VISIBLE, 12, 48, 80, 22, -1);
    mk(L"EDIT", L"100", WS_CHILD | WS_VISIBLE | WS_BORDER, 80, 46, 170, 24, IDC_INPUT);
    mk(L"BUTTON", L"设值", WS_CHILD | WS_VISIBLE, 280, 44, 70, 28, IDC_BTN_SET);
    mk(L"BUTTON", L"加", WS_CHILD | WS_VISIBLE, 360, 44, 70, 28, IDC_BTN_ADD);
    mk(L"BUTTON", L"减", WS_CHILD | WS_VISIBLE, 440, 44, 70, 28, IDC_BTN_SUB);

    mk(L"STATIC", L"随机最小", WS_CHILD | WS_VISIBLE, 12, 90, 80, 22, -1);
    mk(L"EDIT", L"1", WS_CHILD | WS_VISIBLE | WS_BORDER, 80, 88, 90, 24, IDC_MIN);
    mk(L"STATIC", L"随机最大", WS_CHILD | WS_VISIBLE, 180, 90, 80, 22, -1);
    mk(L"EDIT", L"100", WS_CHILD | WS_VISIBLE | WS_BORDER, 250, 88, 90, 24, IDC_MAX);
    mk(L"STATIC", L"间隔ms", WS_CHILD | WS_VISIBLE, 350, 90, 60, 22, -1);
    mk(L"EDIT", L"500", WS_CHILD | WS_VISIBLE | WS_BORDER, 410, 88, 100, 24, IDC_INTERVAL);

    mk(L"BUTTON", L"随机一次", WS_CHILD | WS_VISIBLE, 12, 130, 110, 30, IDC_BTN_RAND);
    mk(L"STATIC", L"次数", WS_CHILD | WS_VISIBLE, 140, 136, 40, 22, -1);
    mk(L"EDIT", L"10", WS_CHILD | WS_VISIBLE | WS_BORDER, 180, 134, 70, 24, IDC_COUNT);
    mk(L"BUTTON", L"连变", WS_CHILD | WS_VISIBLE, 270, 130, 70, 30, IDC_BTN_BURST);
    mk(L"BUTTON", L"自动", WS_CHILD | WS_VISIBLE, 350, 130, 70, 30, IDC_BTN_AUTO);
    mk(L"BUTTON", L"停止", WS_CHILD | WS_VISIBLE, 430, 130, 70, 30, IDC_BTN_STOP);

    mk(L"STATIC", L"", WS_CHILD | WS_VISIBLE, 12, 174, 180, 22, IDC_INFO);
    mk(L"STATIC", L"地址", WS_CHILD | WS_VISIBLE, 200, 174, 40, 22, -1);
    mk(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY, 240, 172, 180, 24, IDC_ADDR);
    mk(L"BUTTON", L"复制地址", WS_CHILD | WS_VISIBLE, 430, 170, 80, 28, IDC_BTN_COPY_ADDR);
    mk(L"STATIC", L"就绪", WS_CHILD | WS_VISIBLE, 12, 204, 520, 22, IDC_STATUS);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            createControls(hwnd);
            refreshUi(hwnd);
            SetTimer(hwnd, kTimerWatch, 80, nullptr);
            return 0;
        case WM_COMMAND:
            onCommand(hwnd, LOWORD(wParam));
            return 0;
        case WM_TIMER:
            if (wParam == kTimerAuto) {
                if (g_state.autoRemaining > 0) {
                    applyRandomDelta();
                    g_state.autoRemaining--;
                    refreshUi(hwnd);
                }
                if (g_state.autoRemaining <= 0) {
                    KillTimer(hwnd, kTimerAuto);
                    setStatus(hwnd, L"自动变化完成");
                }
            } else if (wParam == kTimerWatch) {
                // Always poll and refresh so external memory writes appear in UI immediately.
                refreshUi(hwnd);
            }
            return 0;
        case WM_DESTROY:
            KillTimer(hwnd, kTimerAuto);
            KillTimer(hwnd, kTimerWatch);
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const wchar_t* kClassName = L"MemTransWindow";
    const wchar_t* kIconResName = L"IDI_APP_ICON";

    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = kClassName;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = (HICON)LoadImageW(hInstance, kIconResName, IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
    HICON hIconSmall = (HICON)LoadImageW(hInstance, kIconResName, IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0, kClassName, L"MemTrans (Memory transformer)",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 560, 280,
        nullptr, nullptr, hInstance, nullptr);
    if (!hwnd) return 1;

    if (wc.hIcon) SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)wc.hIcon);
    if (hIconSmall) SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}






