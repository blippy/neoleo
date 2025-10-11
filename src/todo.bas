# 2025-10-10 Added. Works.
# Some simple to-do commands

sub tick
{
	set_cell(0, 3, "\"✅\"")
}

bind("t", "call tick")

