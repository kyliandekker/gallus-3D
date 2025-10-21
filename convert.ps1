# Convert leading groups of spaces to tabs in source files (PowerShell)
# Safe: operates only on leading indentation. Makes .bak backups by default.
# Edit $spacesPerIndent and $exts as needed.

$spacesPerIndent = 4             # change to 2 or whatever your project uses
$exts = '*.cpp','*.c','*.h','*.hpp','*.inl','*.cs','*.py','*.js'  # add/remove file patterns
$createBackups = $true           # set to $false to skip .bak files
Write-Host "Using $spacesPerIndent spaces per indent. Scanning files for: $($exts -join ', ')"

# Build regex for leading groups of N spaces
$spaceGroup = ' ' * $spacesPerIndent
$regexPattern = "^(?:$([regex]::Escape($spaceGroup)))+"

Get-ChildItem -Recurse -Include $exts -File |
ForEach-Object {
    $path = $_.FullName
    try {
        $text = Get-Content -Raw -Encoding UTF8 -ErrorAction Stop $path
    } catch {
        Write-Warning "Skipping (couldn't read): $path"
        return
    }

    $new = [regex]::Replace(
        $text,
        $regexPattern,
        { param($m)
            # number of groups of spaces matched
            $n = [int]($m.Value.Length / $spacesPerIndent)
            # build n tab characters safely
            $tabs = (1..$n | ForEach-Object { [char]9 }) -join ''
            return $tabs
        },
        [System.Text.RegularExpressions.RegexOptions]::Multiline
    )

    if ($new -ne $text) {
        if ($createBackups) {
            Copy-Item -LiteralPath $path -Destination ($path + '.bak') -ErrorAction SilentlyContinue
        }
        # Write back (UTF8 without BOM)
        Set-Content -LiteralPath $path -Value $new -Encoding UTF8
        Write-Host "Converted: $path"
    }
}
Write-Host "Done."