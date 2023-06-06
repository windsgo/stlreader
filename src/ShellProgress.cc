#include "ShellProgress.h"

void
ShellProgress::reset() {
    _print_new_line = true;
    _progress = 0.0;
}

void
ShellProgress::setProgress(float progress) {
    if (progress < 0.0) {
        _progress = 0.0;
    } else if (progress > 1.0) {
        _progress = 1.0;
    } else {
        _progress = progress;
    }
}

::std::ostream&
operator<<(::std::ostream& os, const ShellProgress& shell_progress) {
    

    static auto last_print_time = std::chrono::high_resolution_clock::now();

    auto now = std::chrono::high_resolution_clock::now();

    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_print_time).count();

    if (dur >= shell_progress._print_dur_ms || shell_progress._progress == 1.0 || shell_progress._print_new_line) {
        if (shell_progress._print_new_line) {
            // os << "\n";
            shell_progress._print_new_line = false;
        } else {
            os << "\r";
        }
        
        os << ShellProgress::ProgressFormatter::str(shell_progress._progress);
        os.flush();
        last_print_time = now;
    }


    return os;
}