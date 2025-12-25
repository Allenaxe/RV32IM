#ifndef PIPELINE_H
#define PIPELINE_H

namespace RV32IM {

    template <typename T>
    class PipelineRegister {
        private:
            T in;
            T out;
            bool clear;

        public:
            PipelineRegister() : in(T()), out(T()), clear(false) {}

            void Write(T p_In) {
                in = p_In;
            };

            T Read() {
                return out;
            };

            void Update() {
                // Flush pipeline register if clear signal is set
                if (clear) {
                    in = T();
                    clear = false;
                }

                out = in;
            };

            bool GetClear() {
                return clear;
            };

            void SetClear() {
                clear = true;
            };
    };

}

#endif