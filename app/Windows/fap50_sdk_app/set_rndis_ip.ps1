# Setting parameters
$TargetNameLike = "*RNDIS*"
$IPAddress      = "192.168.100.11"
$PrefixLength   = 24
$Gateway        = "192.168.100.250"
$DNS            = "8.8.8.8"

# Check IsAdmin
$IsAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole("Administrators")

if (-not $IsAdmin) {
    Write-Host "PowerShell is NOT running as Administrator"
} else {
    Write-Host "PowerShell is running as Administrator"
}

# To Find out the target
$adapter = Get-NetAdapter | Where-Object { $_.InterfaceDescription -like $TargetNameLike -and $_.Status -eq "Up" }

if ($adapter) {
    $ifName = $adapter.Name
    Write-Host "Set the interface $ifName IP..."

    # Setting IPv4
    Try {
        New-NetIPAddress -InterfaceAlias $ifName -IPAddress $IPAddress -PrefixLength $PrefixLength -DefaultGateway $Gateway -ErrorAction Stop
        Set-DnsClientServerAddress -InterfaceAlias $ifName -ServerAddresses $DNS -ErrorAction Stop
        Write-Host "Set_successful: $IPAddress/$PrefixLength → GW: $Gateway, DNS: $DNS"
    } Catch {
        Write-Host "Fail to set: " $_.Exception.Message
    }
} else {
    Write-Host "Cannot find the matched target $TargetNameLike"
}
