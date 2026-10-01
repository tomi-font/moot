#include <moot/Input/GamepadLayout.hh>

using SfAxis = sf::Joystick::Axis;

// What a pad reports depends on the driver behind it, not on what is printed on it, so the names are
// resolved through a table per way of reporting.
//
// xinput is the kernel's xpad driver: an Xbox-style pad on USB or on its 2.4G dongle in its X mode, or the
// virtual pad Steam shows a game. joydev numbers the buttons in key-code order, A B X Y LB RB Back Start
// Guide LS RS, and SFML letters the axes by their ABS codes: the left stick on X/Y, the right one on U/V
// (ABS_RX/RY), the left trigger on Z and the right one on R (ABS_RZ).
//
// hid is the kernel's generic HID gamepad: an 8BitDo over Bluetooth, or in its D-input mode. Its buttons
// are the whole BTN_GAMEPAD range in key-code order, so A B X Y are 0 1 3 4 (2 and 5, C and Z, are the
// paddles of an Ultimate 2C), the shoulders 6 7, Back and Start 10 11, Guide 12 and the stick clicks 13 14.
// The right stick is on ABS_Z/ABS_RZ, SFML's Z and R, and the triggers on ABS_GAS/ABS_BRAKE, the accelerator
// being the right one: SFML has no letter of their own for those and reports them as U and V on a pad
// without Rx/Ry. The triggers are reported as buttons 8 and 9 as well, pressed by the pad's own reckoning,
// about a sixth of the pull with some hysteresis. (SDL's database entry for 05000000c82d00001b30000001000000,
// the Ultimate 2C over Bluetooth, the kernel's numbering, and the pad itself.)
constexpr std::array Layouts{
	GamepadLayout("xinput", {0, 1, 2, 3, 4, 5, 6, 7, 9, 10},
	          {SfAxis::X, SfAxis::Y, SfAxis::U, SfAxis::V, SfAxis::Z, SfAxis::R},
	          {std::nullopt, std::nullopt}),
	GamepadLayout("hid", {0, 1, 3, 4, 6, 7, 10, 11, 13, 14},
	          {SfAxis::X, SfAxis::Y, SfAxis::Z, SfAxis::R, SfAxis::V, SfAxis::U},
	          {8, 9}),
};

// xpad exposes exactly the 11 buttons named above and the generic HID path the whole range, 15 or 16.
const GamepadLayout* GamepadLayout::get(const sf::Joystick::Identification&, unsigned buttonCount)
{
	constexpr unsigned XpadButtonCount = 11;
	return &Layouts[buttonCount > XpadButtonCount ? 1 : 0];
}
