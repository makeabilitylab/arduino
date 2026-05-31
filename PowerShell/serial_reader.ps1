# serial_reader.ps1
#
# A simple PowerShell serial reader for debugging serial connections on Windows.
# Unlike our Serial Python programs, this script requires no additional libraries
# or setup — it uses Windows' built-in .NET serial classes, so it's a great way
# to verify a connection works at the OS level before involving Python.
#
# Works with BOTH serial-over-USB and serial-over-Bluetooth:
#
#   Serial over USB (wired):
#     Your Arduino/ESP32 connects via a USB cable. Windows assigns it a COM
#     port backed by a USB-to-UART chip (e.g., "Silicon Labs CP210x" on the
#     Huzzah32, or "CH340"). The baud rate matters here and must match your
#     sketch's Serial.begin() value (e.g., 9600 or 115200).
#     See: https://makeabilitylab.github.io/physcomp/communication/serial-intro.html
#
#   Serial over Bluetooth (wireless):
#     After pairing an ESP32 over Bluetooth Classic (SPP), Windows creates a
#     virtual COM port labeled "Standard Serial over Bluetooth link". From this
#     script's perspective it looks identical to a USB serial port — that's the
#     whole point of SPP! Note: Windows creates TWO such ports per paired device
#     (outgoing + incoming); try them one at a time until you see data. The baud
#     rate is negotiated by the Bluetooth stack, so the -Baud value is ignored
#     for Bluetooth ports (we still pass 115200 for consistency).
#     See: https://makeabilitylab.github.io/physcomp/esp32/bluetooth-serial
#
# Because it talks to any COM port the same way, this script is also a handy
# diagnostic: if it CAN'T receive data over a Bluetooth port, the problem is
# your Bluetooth adapter or driver — not your Python code.
#
# Usage:
#   .\serial_reader.ps1                     # Lists available COM ports
#   .\serial_reader.ps1 -Port COM16         # Reads from COM16 at 115200 baud
#   .\serial_reader.ps1 -Port COM4 -Baud 9600
#
# Press Ctrl+C to exit.
#
# If you get an execution policy error, run this first:
#   Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy RemoteSigned
#
# By Professor Jon E. Froehlich
# Director, Makeability Lab, https://makeabilitylab.cs.uw.edu
# Author, Interactive Physical Computing Textbook, https://makeabilitylab.github.io/physcomp/

param(
    [string]$Port,
    [int]$Baud = 115200
)

function Get-SerialPortInfo {
    # Lists available COM ports with friendly names where available.
    # GetPortNames() only returns bare COM names (e.g., "COM20"), so we
    # query Plug-and-Play devices via CIM/WMI to get human-readable
    # descriptions like "Standard Serial over Bluetooth link (COM20)".
    # This helps you tell your USB cable apart from your Bluetooth port.
    $portNames = [System.IO.Ports.SerialPort]::GetPortNames() | Sort-Object

    if (-not $portNames) {
        Write-Host "  (none found)" -ForegroundColor Yellow
        return
    }

    # Build a lookup of COM name -> friendly name from PnP devices
    $friendly = @{}
    try {
        Get-CimInstance Win32_PnPEntity -ErrorAction Stop |
            Where-Object { $_.Name -match '\(COM\d+\)' } |
            ForEach-Object {
                if ($_.Name -match '\((COM\d+)\)') {
                    $friendly[$matches[1]] = $_.Name
                }
            }
    }
    catch {
        # CIM/WMI unavailable — fall back to bare COM names
    }

    foreach ($p in $portNames) {
        if ($friendly.ContainsKey($p)) {
            # Friendly name already includes "(COMxx)", so print as-is
            Write-Host "  $($friendly[$p])"
        } else {
            Write-Host "  $p"
        }
    }
}

# If no port specified, list available COM ports and exit
if (-not $Port) {
    Write-Host "Available COM ports:" -ForegroundColor Cyan
    Get-SerialPortInfo
    Write-Host ""
    Write-Host "Which port do I pick?" -ForegroundColor Cyan
    Write-Host "  - Your USB cable is usually labeled with a chip name"
    Write-Host "    (e.g., 'Silicon Labs CP210x' or 'CH340')."
    Write-Host "  - Bluetooth ports show as 'Standard Serial over Bluetooth link'."
    Write-Host "    Windows creates two per paired device (outgoing + incoming),"
    Write-Host "    so you may see several. Try them one at a time, starting with"
    Write-Host "    the lowest number, until you see data."
    Write-Host "  - Close Serial Monitor or any other serial program first —"
    Write-Host "    only one program can use a port at a time."
    Write-Host ""
    Write-Host "Usage: .\serial_reader.ps1 -Port COM16 [-Baud 115200]"
    exit
}

Write-Host "Connecting to $Port at $Baud baud..." -ForegroundColor Cyan
Write-Host "(Bluetooth ports may take a few seconds to connect.)"
Write-Host "Press Ctrl+C to exit."
Write-Host ""

try {
    $serial = New-Object System.IO.Ports.SerialPort $Port, $Baud
    $serial.ReadTimeout = 1000
    $serial.DtrEnable = $true
    $serial.Open()

    Write-Host "Connected! Listening for data..." -ForegroundColor Green
    Write-Host ""

    while ($true) {
        try {
            $line = $serial.ReadLine()
            Write-Host $line
        }
        catch [System.TimeoutException] {
            # ReadLine timed out — no data yet, just keep waiting
        }
    }
}
catch [System.UnauthorizedAccessException] {
    Write-Host "Error: $Port is in use by another program." -ForegroundColor Red
    Write-Host "Close Serial Monitor or any other serial terminal and try again."
}
catch {
    Write-Host "Error: Could not open $Port." -ForegroundColor Red
    Write-Host "  $_" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Available COM ports:"
    Get-SerialPortInfo
}
finally {
    if ($serial -and $serial.IsOpen) {
        $serial.Close()
        Write-Host "`nPort closed."
    }
}