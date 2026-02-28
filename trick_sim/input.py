# input.py
#
# Trick simulation startup script.
# This configures integration and sim time.

import trick

trick.stop(10.0)

# Data recording example
trick.add_read(1.0, "position")
