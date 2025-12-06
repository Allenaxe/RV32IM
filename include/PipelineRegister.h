#ifndef PIPELINE_H
#define PIPELINE_H

namespace RV32IM {

    template <typename T>
    class PipelineRegister {
        private:
            T in;
            T out;

        public:
            void Write(T p_In) {
                in = p_In;
            };

            T Read() {
                return out;
            };

            void Update() {
                out = in;
            };

            void Flush() {
                in = T();
                out = T();
            };
    };

}

#endif