#include "VertexArray.hpp"
VertexArray :: VertexArray(int c) {
      glGenVertexArrays(1, &id);
      count = c;
}

VertexArray :: VertexArray(const VertexArray &v) {
      glGenVertexArrays(1, &id);
      // Copy data from the old object
      this->indices = std::map<string, int>(v.indices);
      count = v.count;
      vector<GLuint> temp_buffers(v.buffers.size());
      // Allocate some temporary buffer object handles
      glGenBuffers(v.buffers.size(), &temp_buffers[0]);
      // Copy each old VBO into a new VBO
      int i = 0;
      for (auto &ent : v.buffers) {
            int size = 0;
            glBindBuffer(GL_ARRAY_BUFFER, ent.second);
            glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
            glBindBuffer(GL_COPY_READ_BUFFER, temp_buffers[i]);
            glBufferData(GL_COPY_READ_BUFFER, size, NULL, GL_STATIC_COPY);
            glCopyBufferSubData(GL_ARRAY_BUFFER, GL_COPY_READ_BUFFER, 0, 0,
                        size);
            i++;
      }
      // Copy those temporary buffer objects into our VBOs
      i = 0;
      for (auto &ent : v.buffers) {
            GLuint buffer_id;
            int size = 0;
            int index = indices[ent.first];
            glGenBuffers(1, &buffer_id);
            glBindVertexArray(this->id);
            glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
            glBindBuffer(GL_COPY_READ_BUFFER, temp_buffers[i]);
            glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);
            // Allocate VBO memory and copy
            glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0,
                        size);
            string indexs = ent.first;
            buffers[ent.first] = buffer_id;
            indices[ent.first] = index;
            // Setup the attributes
            size = size / (sizeof(float) * this->count);
            glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(index);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            i++;
      }
      // Delete temporary buffers
      glDeleteBuffers(v.buffers.size(), &temp_buffers[0]);
}
void VertexArray :: addBuffer(string name, int index, vector<vec3> buffer) {
	std::vector<float>control_point_buffer_data;
	for(int i=0; i<buffer.size(); i++){
		control_point_buffer_data.push_back(buffer[i].x);
		control_point_buffer_data.push_back(buffer[i].y);
		control_point_buffer_data.push_back(buffer[i].z);
	}
      addBuffer(name, index, control_point_buffer_data);
}
void VertexArray :: addBuffer(string name, int index, vector<float> buffer) {
      GLuint buffer_id;
      glBindVertexArray(id);

      glGenBuffers(1, &buffer_id);
      glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
      glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float),
                  buffer.data(), GL_STATIC_DRAW);
      buffers[name] = buffer_id;
      indices[name] = index;

      int components = buffer.size() / count;
      glVertexAttribPointer(index, components, GL_FLOAT, GL_FALSE, 0, 0);
      glEnableVertexAttribArray(index);

      // unset states
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
}

void VertexArray :: updateBuffer(string name, vector<vec3> buffer) {
	std::vector<float>control_point_buffer_data;
	for(int i=0; i<buffer.size(); i++){
		control_point_buffer_data.push_back(buffer[i].x);
		control_point_buffer_data.push_back(buffer[i].y);
		control_point_buffer_data.push_back(buffer[i].z);
	}
      updateBuffer(name, control_point_buffer_data);
}
void VertexArray :: updateBuffer(string name, vector<float> buffer) {
      glBindBuffer(GL_ARRAY_BUFFER, buffers[name]);
      glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float),
                  buffer.data(), GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexArray :: ~VertexArray() {
      // cout << "va destroy"<<endl;
      glBindVertexArray(0);
      glDeleteVertexArrays(1, &id);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      for (auto &ent : buffers)
            glDeleteBuffers(1, &ent.second);
}
