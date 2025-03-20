from fourbraillebars import render_braille_progress

"""
Braille Patterns
From the official Unicode Consortium code chart
  	0 	1 	2 	3 	4 	5 	6 	7 	8 	9 	A 	B 	C 	D 	E 	F
U+280x 	⠀ 	⠁ 	⠂ 	⠃ 	⠄ 	⠅ 	⠆ 	⠇ 	⠈ 	⠉ 	⠊ 	⠋ 	⠌ 	⠍ 	⠎ 	⠏
U+281x 	⠐ 	⠑ 	⠒ 	⠓ 	⠔ 	⠕ 	⠖ 	⠗ 	⠘ 	⠙ 	⠚ 	⠛ 	⠜ 	⠝ 	⠞ 	⠟
U+282x 	⠠ 	⠡ 	⠢ 	⠣ 	⠤ 	⠥ 	⠦ 	⠧ 	⠨ 	⠩ 	⠪ 	⠫ 	⠬ 	⠭ 	⠮ 	⠯
U+283x 	⠰ 	⠱ 	⠲ 	⠳ 	⠴ 	⠵ 	⠶ 	⠷ 	⠸ 	⠹ 	⠺ 	⠻ 	⠼ 	⠽ 	⠾ 	⠿
(end of 6-dot cell patterns)
U+284x 	⡀ 	⡁ 	⡂ 	⡃ 	⡄ 	⡅ 	⡆ 	⡇ 	⡈ 	⡉ 	⡊ 	⡋ 	⡌ 	⡍ 	⡎ 	⡏
U+285x 	⡐ 	⡑ 	⡒ 	⡓ 	⡔ 	⡕ 	⡖ 	⡗ 	⡘ 	⡙ 	⡚ 	⡛ 	⡜ 	⡝ 	⡞ 	⡟
U+286x 	⡠ 	⡡ 	⡢ 	⡣ 	⡤ 	⡥ 	⡦ 	⡧ 	⡨ 	⡩ 	⡪ 	⡫ 	⡬ 	⡭ 	⡮ 	⡯
U+287x 	⡰ 	⡱ 	⡲ 	⡳ 	⡴ 	⡵ 	⡶ 	⡷ 	⡸ 	⡹ 	⡺ 	⡻ 	⡼ 	⡽ 	⡾ 	⡿
U+288x 	⢀ 	⢁ 	⢂ 	⢃ 	⢄ 	⢅ 	⢆ 	⢇ 	⢈ 	⢉ 	⢊ 	⢋ 	⢌ 	⢍ 	⢎ 	⢏
U+289x 	⢐ 	⢑ 	⢒ 	⢓ 	⢔ 	⢕ 	⢖ 	⢗ 	⢘ 	⢙ 	⢚ 	⢛ 	⢜ 	⢝ 	⢞ 	⢟
U+28Ax 	⢠ 	⢡ 	⢢ 	⢣ 	⢤ 	⢥ 	⢦ 	⢧ 	⢨ 	⢩ 	⢪ 	⢫ 	⢬ 	⢭ 	⢮ 	⢯
U+28Bx 	⢰ 	⢱ 	⢲ 	⢳ 	⢴ 	⢵ 	⢶ 	⢷ 	⢸ 	⢹ 	⢺ 	⢻ 	⢼ 	⢽ 	⢾ 	⢿
U+28Cx 	⣀ 	⣁ 	⣂ 	⣃ 	⣄ 	⣅ 	⣆ 	⣇ 	⣈ 	⣉ 	⣊ 	⣋ 	⣌ 	⣍ 	⣎ 	⣏
U+28Dx 	⣐ 	⣑ 	⣒ 	⣓ 	⣔ 	⣕ 	⣖ 	⣗ 	⣘ 	⣙ 	⣚ 	⣛ 	⣜ 	⣝ 	⣞ 	⣟
U+28Ex 	⣠ 	⣡ 	⣢ 	⣣ 	⣤ 	⣥ 	⣦ 	⣧ 	⣨ 	⣩ 	⣪ 	⣫ 	⣬ 	⣭ 	⣮ 	⣯
U+28Fx 	⣰ 	⣱ 	⣲ 	⣳ 	⣴ 	⣵ 	⣶ 	⣷ 	⣸ 	⣹ 	⣺ 	⣻ 	⣼ 	⣽ 	⣾ 	⣿
"""


def test_create_braille_progress():
    # Test case 1: Mixed progress values
    str_75_50_25_100 = create_braille_progress(75, 50, 25, 100)
    assert str_75_50_25_100 == "⣿⣿⣟⣛⣛⣉⣉⣁⣀⣀"

    # Test case 2: All at 0%
    str_all_zero = create_braille_progress(0, 0, 0, 0)
    assert str_all_zero == "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀"  # whitespace is 0x2800

    # Test case 3: All at 100%
    str_all_full = create_braille_progress(100, 100, 100, 100)
    assert str_all_full == "⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿"

    # Test case 4: Different partial progress
    str_partial = create_braille_progress(15, 35, 55, 95)
    assert str_partial == "⣿⣷⣶⣦⣤⣄⣀⣀⣀⡀"

    # Test case 5: Edge cases near column boundaries (rounding needed)
    str_boundaries = create_braille_progress(9, 10, 19, 20)
    assert str_boundaries == "⣿⣤⠀⠀⠀⠀⠀⠀⠀⠀"  # whitespace is 0x2800

    # Test case 6: Single progress bar at a time
    str_bar1 = create_braille_progress(50, 0, 0, 0)
    assert str_bar1 == "⠉⠉⠉⠉⠉⠀⠀⠀⠀⠀"  # whitespace is 0x2800

    str_bar2 = create_braille_progress(0, 50, 0, 0)
    assert str_bar2 == "⠒⠒⠒⠒⠒⠀⠀⠀⠀⠀"  # whitespace is 0x2800

    str_bar3 = create_braille_progress(0, 0, 50, 0)
    assert str_bar3 == "⠤⠤⠤⠤⠤⠀⠀⠀⠀⠀" # whitespace is 0x2800

    str_bar4 = create_braille_progress(0, 0, 0, 50)
    assert str_bar4 == "⣀⣀⣀⣀⣀⠀⠀⠀⠀⠀" # whitespace is 0x2800
