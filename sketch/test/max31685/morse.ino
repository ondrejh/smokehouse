int morse_encode_char(char c, uint8_t *m) {
  char uc = c;

  // uppercase
  if ((c >= 'a') && (c <= 'z')) {
    uc = c + ('A' - 'a');
  }

  // find letter
  switch (uc) {
    case 'A':
      *m = 0b01;
      return 2;
    case 'B':
      *m = 0b1000;
      return 4;
    case 'C':
      *m = 0b1010;
      return 4;
    case 'D':
      *m = 0b100;
      return 3;
    case 'E':
      *m = 0b0;
      return 1;
    case 'F':
      *m = 0b0010;
      return 4;
    case 'G':
      *m = 0b110;
      return 3;
    case 'H':
      *m = 0b0000;
      return 4;
    case 'I':
      *m = 0b00;
      return 2;
    case 'J':
      *m = 0b0111;
      return 4;
    case 'K':
      *m = 0b101;
      return 3;
    case 'L':
      *m = 0b0100;
      return 4;
    case 'M':
      *m = 0b11;
      return 2;
    case 'N':
      *m = 0b10;
      return 2;
    case 'O':
      *m = 0b111;
      return 3;
    case 'P':
      *m = 0b0110;
      return 4;
    case 'Q':
      *m = 0b1101;
      return 4;
    case 'R':
      *m = 0b010;
      return 3;
    case 'S':
      *m = 0b000;
      return 3;
    case 'T':
      *m = 0b1;
      return 1;
    case 'U':
      *m = 0b001;
      return 3;
    case 'V':
      *m = 0b0001;
      return 4;
    case 'W':
      *m = 0b011;
      return 3;
    case 'X':
      *m = 0b1001;
      return 4;
    case 'Y':
      *m = 0b1011;
      return 4;
    case 'Z':
      *m = 0b1100;
      return 4;
    case '0':
      *m = 0b11111;
      return 5;
    case '1':
      *m = 0b01111;
      return 5;
    case '2':
      *m = 0b00111;
      return 5;
    case '3':
      *m = 0b00011;
      return 5;
    case '4':
      *m = 0b00001;
      return 5;
    case '5':
      *m = 0b00000;
      return 5;
    case '6':
      *m = 0b10000;
      return 5;
    case '7':
      *m = 0b11000;
      return 5;
    case '8':
      *m = 0b11100;
      return 5;
    case '9':
      *m = 0b11110;
      return 5;
    case '?':
      *m = 0b001100;
      return 6;
    case '!':
      *m = 0b101011;
      return 6;
    case '.':
      *m = 0b010101;
      return 6;
    case ',':
      *m = 0b110011;
      return 6;
    case ';':
      *m = 0b101010;
      return 6;
    case ':':
      *m = 0b111000;
      return 6;
    case '+':
      *m = 0b01010;
      return 5;
    case '-':
      *m = 0b100001;
      return 6;
    case '/':
      *m = 0b10010;
      return 5;
    case '=':
      *m = 0b10001;
      return 5;
    default:
      *m = 0;
      return 0;
  }

  // return letter length
  return 0;
}
