int main(int argc, char* argv[]) {
    initialize_hardware(argc, argv);
    while (!has_wall(FRONT)) {
        move_forward(1);
    }
    while (has_wall(FRONT)) {
        turn_right(1);
    }
    move_forward(1);
    while (has_wall(RIGHT)) {
        move_forward(1);
    }
    turn_right(1);
    while (!has_wall(FRONT)) {
        move_forward(1);
    }
    while (!(has_wall(RIGHT) && has_wall(LEFT) && !has_wall(FRONT))) {
        turn_right(1);
    }
    while (!has_wall(FRONT)) {
        move_forward(1);
    }
    turn_left(1);
    while (!has_wall(FRONT)) {
        move_forward(1);
    } 
    turn_left(1);
    while (!has_wall(FRONT)) {
        move_forward(1);
    } 
    while (has_wall(FRONT)) {
        turn_right(1);
    }
}
