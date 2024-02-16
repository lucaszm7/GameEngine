# Install Dependencies
[CmdletBinding()]
param ()

Write-Host "[LOG] Building Game Engine" -f Blue;

$hasGlobalVcpkg = Get-Command vcpkg;
New-Alias -Name "vcpkgCommand" -Value vcpkg
if (-not ($hasGlobalVcpkg))
{
    Write-Host "[LOG] Doesn't has vcpkg installed." -f DarkYellow;
    Write-Host "[HINT] If you already has vcpkg install, please include in Windows Path Variables." -f Magenta;
    Write-Host "[LOG] Installing vcpkg..." -f Yellow;
    git clone https://github.com/Microsoft/vcpkg.git;
    .\vcpkg\bootstrap-vcpkg.bat;
    .\vcpkg\vcpkg integrate install;

    New-Alias -Name "vcpkgCommand" -Value .\vcpkg\vcpkg
}

Write-Host "[LOG] Installing dependencies..." -f Yellow;
vcpkgCommand install eigen3:x64-windows;
vcpkgCommand install assimp:x64-windows;
vcpkgCommand install glfw3:x64-windows;
vcpkgCommand install glew:x64-windows;
vcpkgCommand install glm:x64-windows;
vcpkgCommand install stb:x64-windows;

if (-not ($?))
{
    Write-Host "[ERROR] Could not install vcpkg or dependencies" -f Red;
    Write-Host "[HINT] This script has to be executed in Developer PowerShell for Visual Studio" -f Magenta;
    exit 1;
}

Write-Host "[LOG] Dependencies installed!" -f Green;
return 0;