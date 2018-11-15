#ifndef DC_VIEW_GLSHADER_H
#define DC_VIEW_GLSHADER_H

#include "ViewDLL.h"

//Qt
class QString;

namespace DCView
	{
		class DC_VIEW_API GLShader
		{
		public:
			//���캯��
			GLShader();
			//��������
			virtual ~GLShader();

			//
			virtual bool FromFile(const QString& shaderBasePath, const QString& shaderBaseFilename);

			//���س���
			virtual bool LoadProgram(const QString& vertShaderFile, const QString& fragShaderFile);

			//����
			virtual void Reset();

			//����
			virtual void Start();
			//ֹͣ
			virtual void Stop();

			//��ȡ��ɫ����
			unsigned GetProgram();


			virtual void SetUniform1i(int loc, int value);
			virtual void SetUniform1f(int loc, float value);
			virtual void SetUniform4fv(int loc, float* value);

			virtual void SetUniform1i(const char* variable, int val);
			virtual void SetUniform2iv(const char* variable, int* p_val);
			virtual void SetUniform3iv(const char* variable, int* p_val);
			virtual void SetUniform4iv(const char* variable, int* p_val);
			virtual void SetTabUniform4iv(const char* uniform, int size, int* p_val);

			virtual void SetUniform1f(const char* variable, float val);
			virtual void SetUniform2fv(const char* variable, float* p_val);
			virtual void SetUniform3fv(const char* variable, float* p_val);
			virtual void SetUniform4fv(const char* variable, float* p_val);
			virtual void SetTabUniform1fv(const char* uniform, int size, float* p_val);
			virtual void SetTabUniform2fv(const char* uniform, int size, float* p_val);
			virtual void SetTabUniform3fv(const char* uniform, int size, float* p_val);
			virtual void SetTabUniform4fv(const char* uniform, int size, float* p_val);
			virtual void SetUniformMatrix4fv(const char* variable, float* p_val, bool transpose = false);

			virtual void SetAttrib4iv(int loc, int* p_val);
			virtual void SetAttrib1f(int loc, float val);
			virtual void SetAttrib2fv(int loc, float* p_val);
			virtual void SetAttrib3fv(int loc, float* p_val);
			virtual void SetAttrib4fv(int loc, float* p_val);

		protected:

			//���ļ��м���
			static unsigned LoadShader(unsigned type, const QString& filename);

			//���ڴ��ж�ȡ
			static QByteArray ReadShaderFile(const QString& filename);

			unsigned prog;
		};
	}
#endif
