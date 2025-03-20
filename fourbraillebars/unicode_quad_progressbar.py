__all__ = ["render_braille_progress"]

def round_to_nearest_5(number):
    """Rounds a number to the nearest multiple of 5."""
    return 5 * round(number / 5)

def render_braille_progress(progress1, progress2, progress3, progress4):
    """Create braille progress bars for four values (0-100)"""

    # Braille patterns for each column (2x4 dots)
    # Each tuple represents the dots to activate for that column
    # (top-left, bottom-left, top-right, bottom-right)
    patterns = []
    progress1 = round_to_nearest_5(progress1)
    progress2 = round_to_nearest_5(progress2)
    progress3 = round_to_nearest_5(progress3)
    progress4 = round_to_nearest_5(progress4)

    for i in range(10):  # 10 columns = 5% per column
        # Calculate which dots should be filled for each progress bar
        # Each column represents 10% (since we have 10 columns for 100%)
        # For partial columns, we only fill the left dot
        # For partial columns, we need to check if the progress is greater than the column start
        # and less than the column end to only fill the left dot
        p1_left = progress1 > i * 10
        p1_right = progress1 >= (i + 1) * 10
        p2_left = progress2 > i * 10
        p2_right = progress2 >= (i + 1) * 10
        p3_left = progress3 > i * 10
        p3_right = progress3 >= (i + 1) * 10
        p4_left = progress4 > i * 10
        p4_right = progress4 >= (i + 1) * 10

        # Special case: if progress is exactly at a column boundary, fill both dots
        if progress1 == (i + 1) * 10:
            p1_left = True
            p1_right = True
        if progress2 == (i + 1) * 10:
            p2_left = True
            p2_right = True
        if progress3 == (i + 1) * 10:
            p3_left = True
            p3_right = True
        if progress4 == (i + 1) * 10:
            p4_left = True
            p4_right = True

        # Braille dots are numbered:
        # 1 4
        # 2 5
        # 3 6
        # 7 8
        # But in Unicode, the bits are ordered differently:
        # Bit 0: Dot 1
        # Bit 1: Dot 2
        # Bit 2: Dot 3
        # Bit 3: Dot 4
        # Bit 4: Dot 5
        # Bit 5: Dot 6
        # Bit 6: Dot 7
        # Bit 7: Dot 8

        # Calculate the correct bit pattern
        dots = 0
        if p1_left: dots |= 0b00000001  # Dot 1
        if p2_left: dots |= 0b00000010  # Dot 2
        if p3_left: dots |= 0b00000100  # Dot 3
        if p1_right: dots |= 0b00001000  # Dot 4
        if p2_right: dots |= 0b00010000  # Dot 5
        if p3_right: dots |= 0b00100000  # Dot 6
        if p4_left: dots |= 0b01000000  # Dot 7
        if p4_right: dots |= 0b10000000  # Dot 8

        patterns.append(chr(0x2800 + dots))

    return ''.join(patterns)
